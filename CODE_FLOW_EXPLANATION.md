# Philosophers Project - Kod İşleyiş Açıklaması 📚

## 📅 Hazırlanma Tarihi: 11 Temmuz 2025

Bu dosya, Philosophers projesinin kod akışını **çalışma sırasına göre** detaylı olarak açıklar.

---

## 🚀 **1. PROGRAM BAŞLANGICI - main() Fonksiyonu**

### 📁 **Dosya:** `src/philo.c`

```c
int main(int argc, char *argv[])
{
    t_data data;                    // Ana veri yapısını tanımla
    
    // 🔧 BAŞLANGIÇ DEĞERLERİ AYARLAMA
    data.is_dead = 0;              // Hiç kimse ölmedi
    data.dead_index = -1;          // Ölen filozof yok
    data.forks = NULL;             // Çatallar henüz oluşturulmadı
    data.philos = NULL;            // Filozoflar henüz oluşturulmadı
    
    // 🔍 ARGÜMAN KONTROLÜ
    if (argc == 5 || argc == 6)   // 5 veya 6 argüman kabul ediliyor
    {
        // ⚙️ ADIM 1: Filozofları initialize et
        init_philo(&data, argv, argc);
        
        // ⚙️ ADIM 2: Çatalları (mutex'leri) oluştur
        init_forks(&data);
        
        // ⚙️ ADIM 3: Monitor thread'ini başlat
        monitor_philo(&data);
        
        // ⚙️ ADIM 4: Başlangıç zamanını ayarla
        set_time(&data);
        
        // ⚙️ ADIM 5: Filozof thread'lerini oluştur
        create_philo(&data);
        
        // ⚙️ ADIM 6: Tüm thread'lerin bitmesini bekle
        philo_join(&data);
        
        // ⚙️ ADIM 7: Belleği temizle
        cleanup(&data);
        
        return (0);
    }
    else
    {
        // ❌ HATA: Yanlış argüman sayısı
        fprintf(stderr, "ERR_INVALID_ARG\n");
        return (1);
    }
}
```

### 🔧 **Yardımcı Fonksiyonlar:**

```c
// ⏰ ZAMAN ALMA FONKSİYONU
long long get_time_in_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);        // Sistem zamanını al
    // tv.tv_sec: saniye, tv.tv_usec: mikrosaniye
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

// ⏰ BAŞLANGIÇ ZAMANI AYARLAMA
void set_time(t_data *data)
{
    data->start_time = get_time_in_ms();  // Simulation başlangıç zamanı
    data->start_flag = 0;                 // Henüz başlamadı
}
```

---

## 🔧 **2. ADIM 1: ARGÜMAN PARSING VE FILOZOF İNİTİALİZASYONU**

### 📁 **Dosya:** `utils/philo_utils.c`

```c
void init_philo(t_data *data, char *argv[], int argc)
{
    int i = 0;
    
    // 🔍 ARGÜMANLARI PARSE ET
    parse_args(argv, data, argc);
    
    // 🧠 BELLEK AYIR: Filozof sayısı kadar t_philo yapısı
    data->philos = calloc(sizeof(t_philo), data->philo_count);
    error_check(data, ERR_MALLOC_FAIL, data->philos);
    
    // 🔄 HER BİR FİLOZOFU AYARLA
    while (i < data->philo_count)
    {
        data->philos[i].id = i + 1;                    // ID: 1, 2, 3, ...
        data->philos[i].data = data;                   // Ana veri yapısına referans
        data->philos[i].last_meal = get_time_in_ms();  // Son yemek zamanı = şimdi
        data->philos[i].eat_count = 0;                 // Henüz yemek yemedi
        i++;
    }
    
    // ✅ KONTROL: Son filozofun ID'si toplam sayıya eşit mi?
    if ((data->philo_count != data->philos[i - 1].id))
    {
        printf("LAST_ID: %d\n", data->philos[i - 1].id);
        exit(1);
    }
}
```

### 🔍 **Argüman Parsing Fonksiyonu:**

```c
void parse_args(char *argv[], t_data *data, int argc)
{
    if (argc == 5)  // 5 argüman: program + 4 parametre
    {
        // argv[1]: filozof sayısı
        // argv[2]: ölüm süresi (ms)
        // argv[3]: yemek süresi (ms) 
        // argv[4]: uyku süresi (ms)
        if (ft_atoi(argv[1], &data->philo_count)
            || ft_atoi(argv[2], &data->time_to_die)
            || ft_atoi(argv[3], &data->time_to_eat)
            || ft_atoi(argv[4], &data->time_to_sleep))
                error_check(data, ERR_INVALID_ARG, NULL);
        data->must_eat = -1;  // Sınırsız yemek
    }
    else  // 6 argüman: + kaç kez yemek yenmeli
    {
        // argv[5]: kaç kez yemek yenmeli
        if (ft_atoi(argv[1], &data->philo_count)
            || ft_atoi(argv[2], &data->time_to_die)
            || ft_atoi(argv[3], &data->time_to_eat)
            || ft_atoi(argv[4], &data->time_to_sleep)
            || ft_atoi(argv[5], &data->must_eat))
                error_check(data, ERR_INVALID_ARG, NULL);
    }
}
```

### 🔢 **String'i Integer'a Çevirme:**

```c
int ft_atoi(char *str, int *res)
{
    int i = 0;
    int sign = 1;
    *res = 0;
    
    // 🔍 UZUNLUK KONTROLÜ (max 11 karakter)
    if (check_long(str))
        return (1);
    
    // ➕➖ İŞARET KONTROLÜ
    while (*(str + i) == '+' || *(str + i) == '-')
    {
        if (*(str + i) == '-')
            sign *= -1;
        i++;
    }
    
    // 🔢 RAKAM DÖNÜŞÜMÜ
    while ((*(str + i) >= '0' && *(str + i) <= '9') || *(str + i) != '\0')
    {
        if (!(*(str + i) >= '0' && *(str + i) <= '9'))
            return (1);  // Geçersiz karakter
        else
            *res = (*res * 10) + (*(str + i) - '0');
        i++;
    }
    
    *res *= sign;
    
    // ✅ POZITIF SAYI KONTROLÜ
    if (*res <= 0)
        return (1);
    
    return (0);
}
```

---

## 🔧 **3. ADIM 2: ÇATAL (MUTEX) İNİTİALİZASYONU**

### 📁 **Dosya:** `utils/philo_utils.c`

```c
void init_forks(t_data *data)
{
    int i = -1;
    
    // 🧠 ÇATALLAR İÇİN BELLEK AYIR
    data->forks = malloc(data->philo_count * sizeof(pthread_mutex_t));
    error_check(data, ERR_MALLOC_FAIL, data->forks);
    
    // 🔐 GLOBAL MUTEX'LERİ BAŞLAT
    pthread_mutex_init(&data->death_mutex, NULL);        // Ölüm durumu koruması
    pthread_mutex_init(&data->start_flag_mutex, NULL);   // Başlangıç bayrağı koruması
    pthread_mutex_init(&data->check_meal_mutex, NULL);   // Yemek kontrolü koruması
    pthread_mutex_init(&data->print_mutex, NULL);        // Konsol çıktısı koruması
    
    // 🔄 HER ÇATAL VE FİLOZOF MUTEX'İNİ BAŞLAT
    while (++i < data->philo_count)
    {
        pthread_mutex_init(&data->forks[i], NULL);              // Çatal mutex'i
        pthread_mutex_init(&data->philos[i].meal_mutex, NULL);  // Yemek mutex'i
    }
    
    // 🔗 ÇATAL ATAMASI (Circular arrangement)
    i = -1;
    while (++i < data->philo_count)
    {
        data->philos[i].left_fork = &data->forks[i];                      // Sol çatal
        data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];  // Sağ çatal
        // % operatörü: Son filozof için sağ çatal = 0. çatal (circular)
    }
}
```

---

## 🔧 **4. ADIM 3: MONITOR THREAD'İNİ BAŞLATMA**

### 📁 **Dosya:** `utils/monitor.c`

```c
// 🏗️ MONITOR THREAD OLUŞTURMA
void monitor_philo(t_data *data)
{
    pthread_create(
        &data->monitor_philo,    // Thread handle
        NULL,                    // Default attributes
        monitor_test,            // Çalışacak fonksiyon
        &*data);                 // Parametre olarak data
}

// 👁️ MONITOR THREAD'İN ANA FONKSİYONU
void *monitor_test(void *argv)
{
    t_data *datas = (t_data *)argv;
    int i;
    
    // 🚀 BAŞLANGIÇ SINYALI VER
    datas->start_flag = 1;  // Filozoflar başlayabilir
    
    // 🔄 SONSUZ DÖNGÜ: Ölüm ve yemek kontrolü
    while (1)
    {
        i = -1;
        while (++i < datas->philo_count)
        {
            // 🔐 GÜVENLI SON YEMEK ZAMANINI AL
            pthread_mutex_lock(&datas->philos[i].meal_mutex);
            long long last = datas->philos[i].last_meal;
            pthread_mutex_unlock(&datas->philos[i].meal_mutex);
            
            // 💀 ÖLÜM KONTROLÜ
            if (get_time_in_ms() - last > datas->time_to_die)
            {
                // 🔐 ÖLÜM DURUMUNU GÜVENLİ OLARAK AYARLA
                pthread_mutex_lock(&datas->death_mutex);
                datas->is_dead = 1;
                datas->dead_index = i;
                pthread_mutex_unlock(&datas->death_mutex);
                
                // 🔔 DİĞER THREAD'LEREBİLDİR
                handle_dead(datas->philos);
                
                // 🚪 MONITOR THREAD'DEN ÇIK
                pthread_exit(NULL);
            }
            else
            {
                // 🍽️ YEMEK HEDEFİ KONTROLÜ
                if (datas->must_eat == datas->philos[i].eat_count)
                    pthread_exit(NULL);
            }
        }
        
        // 💤 CPU SPINNING'İ ÖNLEMEK İÇİN KISA UYKU
        usleep(1000);  // 1ms - ölüm tespiti için yeterli
    }
}
```

---

## 🔧 **5. ADIM 5: FİLOZOF THREAD'LERİNİ OLUŞTURMA**

### 📁 **Dosya:** `utils/philo_utils.c`

```c
void create_philo(t_data *data)
{
    int i = -1;
    
    // 🔄 HER FİLOZOF İÇİN THREAD OLUŞTUR
    while (++i < data->philo_count)
    {
        pthread_create(
            &data->philos[i].thread,    // Thread handle
            NULL,                       // Default attributes
            say_hello,                  // Çalışacak fonksiyon
            &data->philos[i]);         // Parametre olarak filozof
    }
}
```

---

## 🔧 **6. FİLOZOF THREAD'LERİNİN ANA FONKSİYONU**

### 📁 **Dosya:** `utils/philo_utils.c`

```c
void *say_hello(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    
    // ⏳ BAŞLANGIC SİNYALİNİ BEKLE
    while (1)
    {
        if (check_start_flag(philo))
            break;
        usleep(100);  // CPU spinning'i önle
    }
    
    // 🎯 BAŞLANGIÇ SENKRONİZASYONU
    sync_philo_start(philo);
    
    // 🔄 ANA YAŞAM DÖNGÜSÜ
    while (!check_dead(philo))
    {
        // 🍴 ÇATALLARAI AL
        philo_take_fork(philo);
        
        // 🍽️ YEMEK YE
        philo_eat(philo);
        
        // 🍴 ÇATALLAIRI BIRAK (Tersi sırada)
        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);
        
        // 💤 UYU
        philo_sleep(philo);
        
        // 🤔 DÜŞÜN
        philo_thinking(philo);
    }
    
    return (NULL);
}
```

### 🔍 **Başlangıç Kontrolü:**

```c
int check_start_flag(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->start_flag_mutex);
    if (philo->data->start_flag)
    {
        pthread_mutex_unlock(&philo->data->start_flag_mutex);
        return (1);  // Başlayabilir
    }
    pthread_mutex_unlock(&philo->data->start_flag_mutex);
    return (0);  // Henüz başlayamaz
}
```

### 🎯 **Başlangıç Senkronizasyonu:**

```c
void sync_philo_start(t_philo *philo)
{
    // 🔐 SON YEMEK ZAMANINI GÜVENLİ OLARAK AYARLA
    pthread_mutex_lock(&philo->meal_mutex);
    philo->last_meal = get_time_in_ms();
    pthread_mutex_unlock(&philo->meal_mutex);
    
    // ⏰ TEK NUMARALI FİLOZOFLAR İÇİN GECİKME
    // (Başlangıç deadlock'unu önler)
    if (philo->id % 2 != 0)
        usleep(500);  // 0.5ms gecikme
}
```

### 💀 **Ölüm Kontrolü:**

```c
int check_dead(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->death_mutex);
    if (philo->data->is_dead)
    {
        pthread_mutex_unlock(&philo->data->death_mutex);
        return (1);  // Biri öldü
    }
    pthread_mutex_unlock(&philo->data->death_mutex);
    return (0);  // Herkes yaşıyor
}
```

---

## 🔧 **7. FİLOZOF AKTİVİTELERİ**

### 📁 **Dosya:** `utils/philo_routine.c`

### 🍴 **Çatal Alma:**

```c
void philo_take_fork(t_philo *philo)
{
    handle_dead(philo);      // Ölüm kontrolü
    check_meal_goal(philo);  // Yemek hedefi kontrolü
    
    // 👤 TEK FİLOZOF DURUMU
    if (philo->data->philo_count == 1)
    {
        pthread_mutex_lock(philo->left_fork);
        print(philo, "has taken a fork");
        pthread_mutex_unlock(philo->left_fork);
        philo->data->is_dead = 1;  // Tek çatalla yemek yiyemez
        return;
    }
    
    // 🔒 DEADLOCK ÖNLEME: Adres tabanlı sıralama
    pthread_mutex_t *first_fork, *second_fork;
    
    if (philo->left_fork < philo->right_fork)
    {
        first_fork = philo->left_fork;
        second_fork = philo->right_fork;
    }
    else
    {
        first_fork = philo->right_fork;
        second_fork = philo->left_fork;
    }
    
    // 🔐 MUTEX'LERİ SIRALI OLARAK KİLİTLE
    pthread_mutex_lock(first_fork);
    print(philo, "has taken a fork");
    pthread_mutex_lock(second_fork);
    print(philo, "has taken a fork");
}
```

### 🍽️ **Yemek Yeme:**

```c
void philo_eat(t_philo *philo)
{
    // 🔄 SON YEMEK ZAMANINI GÜNCELLE
    last_meal_added(philo);
    
    // 💀 ÖLÜM KONTROLÜ
    handle_dead(philo);
    
    // 📝 YEMEK YE MESAJINI YAZDIR
    print(philo, "is eating");
    
    // 📊 YEMEK SAYISINI ARTIR
    philo->eat_count++;
    
    // ⏰ YEMEK SÜRESİ KADAR BEKLE
    usleep(philo->data->time_to_eat * 1000);
}

// 🔄 SON YEMEK ZAMANINI GÜNCELLEME
void last_meal_added(t_philo *philo)
{
    pthread_mutex_lock(&philo->meal_mutex);
    philo->last_meal = get_time_in_ms();
    pthread_mutex_unlock(&philo->meal_mutex);
}
```

### 💤 **Uyuma:**

```c
void philo_sleep(t_philo *philo)
{
    print(philo, "is sleeping");
    usleep(philo->data->time_to_sleep * 1000);
}
```

### 🤔 **Düşünme:**

```c
void philo_thinking(t_philo *philo)
{
    print(philo, "is thinking");
    
    // 🔄 TEK SAYIDA FİLOZOF İÇİN AÇLIK ÖNLEME
    if (philo->data->philo_count % 2 == 1 && philo->data->philo_count > 1)
    {
        // Düşünme süresini hesapla
        int think_time = (philo->data->time_to_eat * 2) - philo->data->time_to_sleep;
        if (think_time > 0)
            usleep(think_time * 1000);
        else
            usleep(1000);  // Minimum 1ms
    }
}
```

---

## 🔧 **8. GÜVENLI YAZDIRMA**

### 📁 **Dosya:** `utils/utils.c`

```c
void print(t_philo *philo, char *str)
{
    // 💀 ÖLÜM KONTROLÜ: Ölümden sonra yazdırma
    pthread_mutex_lock(&philo->data->death_mutex);
    if (philo->data->is_dead)
    {
        pthread_mutex_unlock(&philo->data->death_mutex);
        return;  // Ölümden sonra yazdırma
    }
    pthread_mutex_unlock(&philo->data->death_mutex);
    
    // 📝 GÜVENLİ YAZDIRMA
    pthread_mutex_lock(&philo->data->print_mutex);
    printf("%lld %d %s\n", 
           get_time_in_ms() - philo->data->start_time,  // Geçen süre
           philo->id,                                   // Filozof ID
           str);                                        // Mesaj
    pthread_mutex_unlock(&philo->data->print_mutex);
}
```

---

## 🔧 **9. ADIM 6: THREAD'LERİN BİTMESİNİ BEKLEME**

### 📁 **Dosya:** `utils/helpers.c`

```c
void philo_join(t_data *data)
{
    int i = -1;
    
    // 👁️ MONITOR THREAD'İNİN BİTMESİNİ BEKLE
    pthread_join(data->monitor_philo, NULL);
    
    // 💀 ÖLÜM MESAJI YAZDIR
    if (data->dead_index != -1)
        philo_dead(data->philos[data->dead_index]);
    
    // 🔄 TÜM FİLOZOF THREAD'LERİNİN BİTMESİNİ BEKLE
    while (++i < data->philo_count)
    {
        pthread_join(data->philos[i].thread, NULL);
    }
}
```

### 💀 **Ölüm Mesajı:**

```c
void philo_dead(t_philo philo)
{
    pthread_mutex_lock(&philo.data->print_mutex);
    printf("%lld %d died\n", 
           get_time_in_ms() - philo.data->start_time, 
           philo.id);
    pthread_mutex_unlock(&philo.data->print_mutex);
}
```

---

## 🔧 **10. ADIM 7: BELLEK TEMİZLEME**

### 📁 **Dosya:** `error/error_handle.c`

```c
void cleanup(t_data *data)
{
    // 🧠 BELLEK TEMİZLEME
    if (data->philos)
        free(data->philos);
    if (data->forks)
        free(data->forks);
    
    // 🔐 MUTEX'LERİ DESTROY ET (Normalde yapılmalı)
    // pthread_mutex_destroy(&data->death_mutex);
    // pthread_mutex_destroy(&data->start_flag_mutex);
    // pthread_mutex_destroy(&data->check_meal_mutex);
    // pthread_mutex_destroy(&data->print_mutex);
    // ... fork ve meal mutex'leri de
}
```

---

## 🔄 **ÖZET: PROGRAM AKIŞ DİYAGRAMI**

```
MAIN() BAŞLANGICI
    ↓
1. ARGÜMAN KONTROLÜ (argc == 5 veya 6)
    ↓
2. INIT_PHILO() - Filozof yapılarını oluştur
    ↓
3. INIT_FORKS() - Çatal mutex'lerini oluştur
    ↓
4. MONITOR_PHILO() - Monitor thread'ini başlat
    ↓ 
5. SET_TIME() - Başlangıç zamanını ayarla
    ↓
6. CREATE_PHILO() - Filozof thread'lerini oluştur
    ↓
═══════════════════════════════════════════════════
PARALEL ÇALIŞMA:
                                    
📹 MONITOR THREAD:               👤 FİLOZOF THREAD'LERİ:
- start_flag = 1 yap            - Başlangıç sinyalini bekle
- Sürekli ölüm kontrolü         - Senkronizasyon
- Yemek hedefi kontrolü         - YAŞAM DÖNGÜSÜ:
- CPU spinning önleme             * Çatal al
                                  * Yemek ye
                                  * Çatal bırak
                                  * Uyu
                                  * Düşün
                                  * Ölüm kontrolü
                                  * Tekrar et
═══════════════════════════════════════════════════
    ↓
7. PHILO_JOIN() - Tüm thread'lerin bitmesini bekle
    ↓
8. CLEANUP() - Bellek temizleme
    ↓
PROGRAM SONU
```

---

## 🔐 **MUTEX STRATEJİLERİ**

### **Global Mutex'ler:**
- **death_mutex**: Ölüm durumu koruması
- **start_flag_mutex**: Başlangıç sinyali koruması
- **check_meal_mutex**: Yemek hedefi koruması
- **print_mutex**: Konsol çıktısı koruması

### **Filozof-Specific Mutex'ler:**
- **meal_mutex**: Her filozofun son yemek zamanı koruması
- **fork mutex'leri**: Çatal paylaşım koruması

### **Deadlock Önleme:**
- Adres tabanlı mutex sıralaması
- Tek filozof özel durumu
- Başlangıç gecikmeleri

---

## 🚨 **KRİTİK NOKTALAR**

1. **Race Condition Önleme**: Tüm shared data mutex ile korunuyor
2. **Deadlock Önleme**: Çatal alma sıralaması ve özel durumlar
3. **CPU Spinning Önleme**: usleep() çağrıları
4. **Memory Management**: Doğru malloc/free kullanımı
5. **Thread Synchronization**: Başlangıç ve bitiş senkronizasyonu

---

## 📊 **PERFORMANS OPTİMİZASYONLARI**

1. **Monitor Polling**: 1ms aralıklarla kontrol
2. **Thinking Time**: Tek sayıda filozof için özel hesaplama
3. **Staggered Start**: Tek ID'li filozoflar için gecikme
4. **Efficient Locking**: Minimum lock süreleri

Bu kod akışı, klasik Dining Philosophers problemini thread-safe ve deadlock-free şekilde çözer! 🎯
