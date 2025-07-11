# Philosophers Project - TODO List 📋

## 📅 **Kontrol Tarihi:** 11 Temmuz 2025

PDF gereksinimlerine göre kodumuzda eksik olan ve düzeltilmesi gereken konular:

## 🎯 **TEST SONUÇLARI (11 Temmuz 2025)**

### ✅ **Çalışan Özellikler:**
- [x] Log formatı doğru: `"timestamp_in_ms X has taken a fork"` ✅
- [x] Çatal alma logları mevcut ✅ 
- [x] 5. argüman desteği (must_eat) çalışıyor ✅
- [x] Tek filozof durumu doğru şekilde handle ediliyor ✅
- [x] 200 filozof ile test başarılı ✅
- [x] Death detection çalışıyor ✅

### ❌ **Tespit Edilen Sorunlar:**


## 🔧 **1. ÖLÜM MESAJI YAZDIRMA PROBLEMİ (KRİTİK)**

### ❌ **Mevcut Durum:**
```c
// monitor.c - Ölüm mesajı comment'te
//pthread_mutex_lock(&datas->print_mutex);
//printf("%lld %d died\n", get_time_in_ms() - datas->start_time, datas->philos[i].id);
//pthread_mutex_unlock(&datas->print_mutex);
```

### 🎯 **Problem:**
- Death detection çalışıyor ama death message yazılmıyor
- `philo_dead()` fonksiyonu sadece `philo_join()` içinde çağrılıyor
- Gerçek zamanlı ölüm mesajı yok

### ✅ **Çözüm:**
- [ ] Monitor içindeki death message comment'ini aç
- [ ] Ya da death detection anında print yap

---

## 🔧 **2. MEAL COUNT STOP LOGIC EKSİK (ÖNEMLİ)**

### ❌ **Mevcut Durum:**
```c
// monitor.c - Sadece check var ama stop logic eksik
if (datas->must_eat == datas->philos[i].eat_count)
    pthread_exit(NULL);
```

### 🎯 **Problem:**
- Tek filozof must_eat sayısına ulaştığında exit yapıyor
- Ama TÜM filozofların ulaşmasını beklemeli
- Simulation durmalı ama sadece o thread exit yapıyor

### ✅ **Çözüm:**
- [ ] Tüm filozofların must_eat'e ulaşıp ulaşmadığını kontrol et
- [ ] Global counter ekle
- [ ] Tüm filozoflar hedefi tutturduğunda simulation durdur

---

## 🔧 **3. MUTEX UNLOCK EKSIKLIĞI (KRİTİK)**

### ❌ **Tespit Edilen:**
```c
// philo_routine.c - philo_take_fork() sonrası unlock eksik
// Single philosopher case'de:
pthread_mutex_lock(philo->left_fork);
print(philo, "has taken a fork");
pthread_mutex_unlock(philo->left_fork);  // ✅ Var
// Normal case'de unlock philo_utils.c'de yapılıyor ✅
```

### ✅ **Durum:** 
- Mutex unlock'lar doğru yapılıyor ✅

---

## 🔧 **4. PERFORMANCE İYİLEŞTİRMELERİ**

### 🎯 **Potansiyel İyileştirmeler:**
- [ ] Monitor polling süresini test et (şu an 1ms)
- [ ] Thinking time optimizasyonu test et
- [ ] Memory allocation kontrolü

---

## 🔧 **5. EDGE CASE TESTLERİ**

### ✅ **Test Edilenler:**
- [x] `./philo 1 800 200 200` - Tek filozof ✅
- [x] `./philo 4 310 200 100` - Normal durum ✅  
- [x] `./philo 200 800 200 200` - Çok filozof ✅
- [x] `./philo 5 800 200 200 7` - Must eat ✅

### ❌ **Test Edilmesi Gerekenler:**
- [ ] Death timing precision test
- [ ] Memory leak test (valgrind)
- [ ] Data race test (helgrind)

---

## 📝 **DETAYLI TEST SONUÇLARI**

### ✅ **Başarılı Testler:**
```bash
# Test 1: Normal durum (ölüm yok)
./philo 4 310 200 100
✅ Çalışıyor - Log formatı doğru

# Test 2: Tek filozof  
./philo 1 800 200 200
✅ Çalışıyor - Tek çatal alıp bekliyor

# Test 3: 5. argüman
./philo 5 800 200 200 7  
✅ Çalışıyor - Must eat parsing yapıyor

# Test 4: Çok filozof
./philo 200 800 200 200
✅ Çalışıyor - No deadlock, smooth execution
```

### ❌ **Problem Alanları:**
```bash
# Test 5: Death scenario
./philo 4 200 100 100
❌ Ölüm mesajı yazılmıyor (monitor'da comment)

# Test 6: Must eat stop logic
./philo 2 800 200 200 2
❌ 2 kez yedikten sonra durmaz (individual exit var)
```

---

## 🔧 **HİZLI FİX ÖNERİLERİ**

### 1. **Monitor Death Message Fix:**
```c
// monitor.c line 32-35 - Comment'i aç:
pthread_mutex_lock(&datas->print_mutex);
printf("%lld %d died\n", get_time_in_ms() - datas->start_time, datas->philos[i].id);
pthread_mutex_unlock(&datas->print_mutex);
```

### 2. **Must Eat Global Counter:**
```c
// t_data struct'a ekle:
int finished_eating_count;

// Monitor'da:
if (all_philosophers_finished_eating(datas))
    stop_simulation(datas);
```

**🎯 Sonuç:** Kod %85 hazır, sadece 2 kritik fix kaldı!

---

## 📊 **ÖNCELİK SIRASI (11 Temmuz 2025)**

### 🔴 **KRİTİK (Hemen Yapılmalı):**
1. **Ölüm Mesajı Yazma** - Monitor'da comment açılmalı
2. **Must Eat Logic** - Tüm filozoflar için kontrol

### 🟡 **ÖNEMLİ (Yakında):**
3. **Performance testleri** - Death timing, memory leaks
4. **Edge case testleri** - Valgrind, helgrind

### 🟢 **TAMAMLANDI:**
5. ✅ **Log Format** - PDF compliance ✅
6. ✅ **5. Argüman** - Must eat desteği ✅  
7. ✅ **Tek Filozof** - Doğru handling ✅
8. ✅ **Fork Logging** - "has taken a fork" ✅

---

## 🎯 **HEDEF DURUM**

### ✅ **Şu Anki Working Example:**
```bash
./philo 4 310 200 100
1 2 has taken a fork
1 2 has taken a fork  
1 2 is eating
1 4 has taken a fork
201 2 is sleeping
# ÇALIŞIYOR ✅
```

### ❌ **Eksik:**
- Ölüm mesajı gerçek zamanlı yazılmıyor
- Must eat tüm filozof kontrolü yok

**📅 Güncel Durum:** PDF compliance ~85%  
**🎯 Hedef Tamamlama:** 12 Temmuz 2025


# mutex init oluştu mu check & thread oluştu mu check
# Philo ölüm yazdırmayı monitor içinden alınacak joinlerden sonra ya da philoların fonksiyonun da yazdırılacak (Oldu gibi ama bi kontrol edilecek)
# ./philo 200 800 200 200    Ölmemesi gerek ölüyor
# $ ./philo 200 401 200 200     çok geç ölüm mesajı yazıyor
# Ölüm mesajı bazen 100-200 ms geç yazılıyor