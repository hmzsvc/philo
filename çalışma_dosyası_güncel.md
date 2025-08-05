# Philosophers Projesi - Detaylı Kod Analizi (Güncel Versiyon - pthread_exit'siz)

## 📖 İçindekiler
1. [Proje Genel Bakış](#proje-genel-bakış)
2. [Dosya Yapısı](#dosya-yapısı)
3. [Veri Yapıları](#veri-yapıları)
4. [Program Akışı](#program-akışı)
5. [Fonksiyon Analizleri](#fonksiyon-analizleri)
6. [Thread Yönetimi](#thread-yönetimi)
7. [Mutex Kullanımı](#mutex-kullanımı)
8. [Hata Yönetimi](#hata-yönetimi)
9. [pthread_exit Kaldırma Süreci](#pthread_exit-kaldırma-süreci)
10. [Deadlock Önleme Teknikleri](#deadlock-önleme-teknikleri)

---

## Proje Genel Bakış

**Philosophers** projesi, klasik "Dining Philosophers" problemini çözen bir C programıdır. Bu problem, eşzamanlılık (concurrency) ve deadlock önleme konularını öğretmek için kullanılan ünlü bir bilgisayar bilimi problemidir.

### Problem Tanımı:
- N adet filozof bir masanın etrafında oturuyor
- Her filozofun yanında birer çatal var
- Filozoflar sadece 3 şey yapıyor: yemek yeme, uyuma, düşünme
- Yemek yemek için 2 çatala ihtiyaç var (sol ve sağ)
- Filozoflar ölmemeli (açlıktan)

### Güncel Özellikler:
- **pthread_exit kullanılmıyor** - 42 norm uyumlu
- **Gelişmiş deadlock önleme** - Farklı timing stratejileri
- **Thread-safe resource management** - Güvenli mutex unlock
- **Optimized synchronization** - Çift/tek filozof ayrımı

---

## Dosya Yapısı

```
philo/
├── src/
│   └── philo.c              # Ana program dosyası
├── utils/
│   ├── helpers.c            # Yardımcı fonksiyonlar
│   ├── monitor.c            # İzleme thread'i
│   ├── monitor_utils.c      # İzleme yardımcıları
│   ├── mutex_utils.c        # Mutex yönetimi (pthread_exit'siz)
│   ├── philo_routine.c      # Filozof rutinleri
│   ├── philo_utils.c        # Filozof yardımcıları (güncellenmiş)
│   └── utils.c              # Genel yardımcılar
├── error/
│   └── error_handle.c       # Hata yönetimi
├── lib/
│   ├── philo.h              # Ana header dosyası
│   └── error.h              # Hata kodları
└── Makefile                 # Derleme dosyası
```

---

## Veri Yapıları

### 1. `t_philo` Yapısı (Filozof)
```c
typedef struct s_philo
{
    int                identity;            // Filozofun kimliği (1, 2, 3...)
    int                left_fork_bool;      // Sol çatal kullanımda mı?
    int                right_fork_bool;     // Sağ çatal kullanımda mı?
    int                eat_count;           // Kaç kez yemek yedi
    long long          last_meal_time;      // Son yemek zamanı
    pthread_t          thread;              // Filozofun thread'i
    pthread_mutex_t    *left_fork;          // Sol çatal mutex'i
    pthread_mutex_t    *right_fork;         // Sağ çatal mutex'i
    pthread_mutex_t    eat_count_mutex;     // Yemek sayısı mutex'i
    t_table            *data;               // Ana veri yapısına pointer
} t_philo;
```

### 2. `t_table` Yapısı (Oyun Durumu)
```c
typedef struct s_table
{
    int                philo_count;         // Filozof sayısı
    int                dead_index;          // Ölen filozofun index'i
    int                must_eat;            // Zorunlu yemek sayısı
    int                is_dead;             // Ölüm durumu (0,1,2,3)
    int                time_to_die;         // Ölüm süresi (ms)
    int                time_to_eat;         // Yemek süresi (ms)
    int                time_to_sleep;       // Uyku süresi (ms)
    int                start_flag;          // Başlama sinyali
    int                must_meal_loop;      // Yemek hedefine ulaşan sayısı
    long long          start_time;          // Başlangıç zamanı
    long long          last_meal_philo;     // Son yemek zamanı
    pthread_t          monitor_philo;       // İzleyici thread
    pthread_mutex_t    *forks;              // Çatal mutex'leri
    pthread_mutex_t    death_mutex;         // Ölüm mutex'i
    pthread_mutex_t    check_meal_mutex;    // Yemek kontrol mutex'i
    pthread_mutex_t    start_flag_mutex;    // Başlama mutex'i
    pthread_mutex_t    print_mutex;         // Yazdırma mutex'i
    pthread_mutex_t    meal_mutex;          // Yemek zamanı mutex'i
    pthread_mutex_t    must_meal_mutex;     // Zorunlu yemek mutex'i
    t_philo            *philos;             // Filozof dizisi
} t_table;
```

### 3. Hata Kodları
```c
typedef enum s_error_code
{
    ERR_MALLOC_FAIL = 1,    // Bellek tahsisi hatası
    ERR_INVALID_ARG = 2,    // Geçersiz argüman
    ERR_THREAD_FAIL = 3,    // Thread oluşturma hatası
    ERR_ATOI_FAIL = 4,      // String'den integer dönüştürme hatası
} t_error_code;
```

---

## Program Akışı

### 1. Program Başlangıcı (`main` - philo.c)

```c
int main(int argc, char *argv[])
{
    t_table data;

    if (argc == 5 || argc == 6)  // Argüman kontrolü
    {
        initialize_table(&data, argv, argc);   // 1. Veri yapısını başlat
        initialize_forks(&data);               // 2. Mutex'leri başlat
        setup_philosopher_monitor(&data);      // 3. İzleyici thread'i oluştur
        create_philo(&data);                   // 4. Filozof thread'lerini oluştur
        philo_join(&data);                     // 5. Thread'leri bekle
        reset_table(&data);                    // 6. Temizlik yap
        return (0);
    }
    else
    {
        printf("ERR_INVALID_ARG\n");
        return (1);
    }
}
```

### 2. Akış Sırası

1. **Argüman Kontrolü**: 5 veya 6 argüman olmalı
2. **Veri Başlatma**: Struct'ları ve değerleri başlat
3. **Mutex Başlatma**: Bütün mutex'leri oluştur
4. **İzleyici Oluşturma**: Ölüm kontrolü için thread oluştur
5. **Filozof Oluşturma**: Her filozof için thread oluştur
6. **Başlatma**: Bütün thread'leri senkronize başlat
7. **Bekleme**: Thread'lerin bitmesini bekle
8. **Temizlik**: Bellek ve mutex'leri temizle

---

## Fonksiyon Analizleri

### 📂 utils/philo_utils.c

#### `initialize_table` Fonksiyonu
```c
void initialize_table(t_table *data, char *argv[], int argc)
```
- **Amaç**: Ana veri yapısını ve filozof dizisini başlat
- **Adımlar**:
  1. Argümanları parse et
  2. Pointer'ları NULL'la başlat
  3. Başlangıç değerlerini ata
  4. Filozof dizisi için bellek tahsis et
  5. Her filozof için başlangıç değerlerini ata

#### `create_philo` Fonksiyonu
```c
void create_philo(t_table *data)
```
- **Amaç**: Filozof thread'lerini oluştur
- **Akış**:
  1. Her filozof için thread oluştur
  2. Thread oluşturma hatası varsa hata flag'i set et
  3. Başlangıç zamanını kaydet
  4. Başlama sinyali ver

#### `initialize_forks` Fonksiyonu
```c
void initialize_forks(t_table *data)
```
- **Amaç**: Bütün mutex'leri başlat
- **Başlatılan Mutex'ler**:
  1. `death_mutex`: Ölüm durumu
  2. `start_flag_mutex`: Başlama sinyali
  3. `check_meal_mutex`: Yemek kontrol
  4. `print_mutex`: Yazdırma
  5. `must_meal_mutex`: Zorunlu yemek
  6. `meal_mutex`: Yemek zamanı
  7. Çatal mutex'leri (her çatal için)
  8. Filozof yemek sayısı mutex'leri

#### `philo_process` Fonksiyonu (static) - **GÜNCEL VERSİYON**
```c
static void *philo_process(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    
    // Başlama kontrolü
    while (1)
    {
        if (check_start_flag(philo))
            break;
        usleep(100);
    }
    
    one_philo_handle(philo);
    sync_philo_start(philo);
    
    // Ana döngü - pthread_exit'siz güvenli versiyon
    while (!check_dead(philo))
    {
        // Yemek hedefi kontrolü
        if (philo->data->must_eat != -1 && check_meal_goal(philo))
            break;
        
        // Ölüm kontrolü
        if (handle_dead(philo))
            break;
            
        philo_take_fork(philo);
        philo_eat(philo);
        
        // Güvenli mutex unlock
        if (philo->right_fork_bool)
        {
            pthread_mutex_unlock(philo->right_fork);
            philo->right_fork_bool = 0;
        }
        if (philo->left_fork_bool)
        {
            pthread_mutex_unlock(philo->left_fork);
            philo->left_fork_bool = 0;
        }
        
        // Tekrar kontroller
        if (philo->data->must_eat != -1 && check_meal_goal(philo))
            break;
        if (handle_dead(philo))
            break;
            
        philo_sleep(philo);
        
        if (handle_dead(philo))
            break;
            
        philo_thinking(philo);
    }
    
    return (NULL);  // pthread_exit(NULL) yerine return NULL
}
```

### 📂 utils/philo_routine.c

#### `philo_eat` Fonksiyonu
```c
void philo_eat(t_philo *philo)
{
    last_meal_added(philo);
    print(philo, "is eating");
    pthread_mutex_lock(&philo->eat_count_mutex);
    philo->eat_count++;
    pthread_mutex_unlock(&philo->eat_count_mutex);
    ft_usleep(philo->data->time_to_eat, philo);
}
```
- **Amaç**: Filozof yemek yer
- **Adımlar**:
  1. Son yemek zamanını güncelle
  2. "is eating" mesajını yazdır
  3. Yemek sayısını artır (mutex ile)
  4. Yemek süresince bekle

#### `philo_sleep` Fonksiyonu
```c
void philo_sleep(t_philo *philo)
{
    print(philo, "is sleeping");
    ft_usleep(philo->data->time_to_sleep, philo);
}
```

#### `philo_thinking` Fonksiyonu - **GÜNCEL VERSİYON**
```c
void philo_thinking(t_philo *philo)
{
    print(philo, "is thinking");
    usleep(philo->data->time_to_die - (philo->data->time_to_eat + philo->data->time_to_sleep));
}
```
- **Amaç**: Filozof düşünür
- **Hesaplama**: `time_to_die - (time_to_eat + time_to_sleep)` kadar bekle
- **Not**: Basit hesaplama kullanılıyor

#### `philo_take_fork` Fonksiyonu - **GÜNCEL VERSİYON**
```c
void philo_take_fork(t_philo *philo)
{
    if (philo->identity % 2 == 0)
    {
        pthread_mutex_lock(philo->left_fork);
        philo->left_fork_bool = 1;
        print(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        philo->right_fork_bool = 1;
        print(philo, "has taken a fork");
    }
    else
    {
        pthread_mutex_lock(philo->right_fork);
        philo->right_fork_bool = 1;
        print(philo, "has taken a fork");
        usleep(20);  // Deadlock önleme gecikme
        pthread_mutex_lock(philo->left_fork);
        philo->left_fork_bool = 1;
        print(philo, "has taken a fork");
    }
}
```
- **Deadlock Önleme**: Çift/tek ID'ye göre farklı sıralama
- **Gecikme**: Tek ID'li filozoflar için 20µs gecikme

### 📂 utils/mutex_utils.c

#### `check_dead` Fonksiyonu
```c
int check_dead(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->death_mutex);
    if (philo->data->is_dead == 1 || philo->data->is_dead == 2 || philo->data->is_dead == 3)
    {
        pthread_mutex_unlock(&philo->data->death_mutex);
        return (1);
    }
    pthread_mutex_unlock(&philo->data->death_mutex);
    return (0);
}
```

#### `handle_dead` Fonksiyonu - **GÜNCEL VERSİYON**
```c
int handle_dead(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->death_mutex);
    if (philo->data->is_dead == 1)
    {
        pthread_mutex_unlock(&philo->data->death_mutex);
        if (philo->right_fork_bool)
        {
            pthread_mutex_unlock(philo->right_fork);
            philo->right_fork_bool = 0;
        }
        if (philo->left_fork_bool)
        {
            pthread_mutex_unlock(philo->left_fork);
            philo->left_fork_bool = 0;
        }
        return (1);  // Thread sonlansın
    }
    pthread_mutex_unlock(&philo->data->death_mutex);
    return (0);  // Devam etsin
}
```
- **pthread_exit yerine return value**
- **Güvenli fork cleanup**: Bool kontrolü ile

#### `check_meal_goal` Fonksiyonu - **GÜNCEL VERSİYON**
```c
int check_meal_goal(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->check_meal_mutex);
    if (philo->eat_count == philo->data->must_eat)
    {
        pthread_mutex_lock(&philo->data->must_meal_mutex);
        philo->data->must_meal_loop++;
        pthread_mutex_unlock(&philo->data->must_meal_mutex);
        pthread_mutex_unlock(&philo->data->check_meal_mutex);
        if (philo->left_fork_bool)
            pthread_mutex_unlock(philo->left_fork);
        if (philo->right_fork_bool)
            pthread_mutex_unlock(philo->right_fork);
        philo->left_fork_bool = 0;
        philo->right_fork_bool = 0;
        return (1);  // Thread sonlansın
    }
    pthread_mutex_unlock(&philo->data->check_meal_mutex);
    return (0);  // Devam etsin
}
```

### 📂 utils/monitor_utils.c

#### `check_and_handle_death` Fonksiyonu - **GÜNCEL VERSİYON**
```c
int check_and_handle_death(t_table *data, int philo_index)
{
    pthread_mutex_lock(&data->meal_mutex);
    data->last_meal_philo = data->philos[philo_index].last_meal_time;
    pthread_mutex_unlock(&data->meal_mutex);
    if (get_time_in_ms() - data->last_meal_philo >= data->time_to_die)
    {
        set_death_status(data, philo_index);
        return (1);  // Monitor thread sonlansın
    }
    else if (check_dead(data->philos))
        return (1);
    return (0);  // Devam etsin
}
```

### 📂 utils/utils.c

#### `sync_philo_start` Fonksiyonu - **GÜNCEL VERSİYON**
```c
void sync_philo_start(t_philo *philo)
{
    pthread_mutex_lock(&philo->data->meal_mutex);
    philo->last_meal_time = get_time_in_ms();
    pthread_mutex_unlock(&philo->data->meal_mutex);
    if (philo->identity % 2 != 0)
        ft_usleep(philo->data->time_to_eat, philo);
}
```
- **Senkronizasyon**: Tek ID'li filozoflar gecikme ile başlar

---

## Thread Yönetimi

### 1. Thread Türleri

#### Ana Thread (main)
- Program başlatır
- Diğer thread'leri oluşturur
- Thread'lerin bitmesini bekler
- Temizlik yapar

#### İzleyici Thread (monitor_philo)
- Filozofları sürekli izler
- Ölüm durumlarını kontrol eder
- Yemek hedefini kontrol eder
- **pthread_exit yerine return NULL** kullanır

#### Filozof Thread'leri (philo_process)
- Her filozof için bir thread
- Ana yaşam döngüsünü işletir
- **Güvenli break kullanımı** ile sonlanır

### 2. Thread Sonlandırma - **GÜNCEL YAKLAŞIM**

```c
// Ana döngüde pthread_exit yerine break kullanımı
while (!check_dead(philo))
{
    if (handle_dead(philo))
        break;  // pthread_exit yerine
    
    // İşlemler...
    
    if (check_meal_goal(philo))
        break;  // pthread_exit yerine
}
return (NULL);  // pthread_exit(NULL) yerine
```

---

## Mutex Kullanımı

### 1. Çatal Mutex'leri (`forks`)
- **Amaç**: Her çatalın tekil kullanımını garanti et
- **Deadlock Önleme**: Çift/tek ID farklı sıralama
- **Güvenli Unlock**: Bool kontrolü ile

### 2. Ölüm Mutex'i (`death_mutex`)
- **Koruduğu Değişkenler**: `is_dead`, `dead_index`
- **Kullanım**: Thread-safe ölüm kontrolü

### 3. Yemek Mutex'leri
```c
pthread_mutex_t meal_mutex;        // Son yemek zamanı
pthread_mutex_t check_meal_mutex;  // Yemek kontrolü
pthread_mutex_t must_meal_mutex;   // Zorunlu yemek sayacı
```

### 4. **Güvenli Mutex Unlock Stratejisi**
```c
if (philo->right_fork_bool)
{
    pthread_mutex_unlock(philo->right_fork);
    philo->right_fork_bool = 0;
}
if (philo->left_fork_bool)
{
    pthread_mutex_unlock(philo->left_fork);
    philo->left_fork_bool = 0;
}
```

---

## pthread_exit Kaldırma Süreci

### 1. Problem ve Çözüm

#### Eski Yaklaşım (pthread_exit ile)
```c
void handle_dead(t_philo *philo)
{
    // Kontroller...
    pthread_exit(NULL);  // İzin verilmeyen
}
```

#### Yeni Yaklaşım (return value ile)
```c
int handle_dead(t_philo *philo)
{
    // Kontroller...
    return (1);  // Thread sonlansın
}
```

### 2. Ana Döngü Değişikliği

#### Eski
```c
while (1)
{
    // İşlemler...
    handle_dead(philo);  // İçinde pthread_exit var
}
```

#### Yeni
```c
while (!check_dead(philo))
{
    if (handle_dead(philo))
        break;  // Güvenli çıkış
    // İşlemler...
}
return (NULL);
```

---

## Deadlock Önleme Teknikleri

### 1. Fork Alma Stratejisi
```c
// Çift ID: Sol-Sağ sırası
if (philo->identity % 2 == 0)
{
    pthread_mutex_lock(philo->left_fork);
    pthread_mutex_lock(philo->right_fork);
}
// Tek ID: Sağ-Sol sırası + gecikme
else
{
    pthread_mutex_lock(philo->right_fork);
    usleep(20);  // Deadlock önleme
    pthread_mutex_lock(philo->left_fork);
}
```

### 2. Timing Optimizasyonu
- **Tek Filozoflar**: `time_to_eat` kadar gecikme ile başlama
- **Düşünme Zamanı**: Hesaplanmış optimal süre
- **Mikro Gecikmeler**: Deadlock kırma için

### 3. Resource Management
- **Bool Kontrolü**: Mutex unlock öncesi kontrol
- **Cleanup**: Hata durumunda resource temizliği
- **Thread-Safe**: Bütün işlemler mutex korumalı

---

## Test Senaryoları

### 1. Normal Testler
```bash
./philo 5 800 200 200     # Standart test
./philo 4 410 200 200     # Dar timing
./philo 3 310 200 100     # Hızlı test
```

### 2. Edge Cases
```bash
./philo 1 800 200 200     # Tek filozof
./philo 2 310 200 100     # İki filozof
./philo 100 800 200 200   # Çok filozof
```

### 3. Meal Goal Testleri
```bash
./philo 5 800 200 200 3   # 3 kez yeme hedefi
./philo 4 410 200 200 2   # 2 kez yeme hedefi
```

---

## Debugging ve Optimizasyon

### 1. Valgrind Kontrolleri
```bash
valgrind --tool=helgrind ./philo 4 410 200 200
valgrind --tool=memcheck --leak-check=full ./philo 5 800 200 200
```

### 2. Thread Sanitizer
```bash
gcc -fsanitize=thread -o philo *.c
./philo 3 310 200 200
```

### 3. Performance Monitoring
- **Timing Kontrolü**: Hassas zaman ölçümü
- **Resource Usage**: CPU ve memory kullanımı
- **Deadlock Detection**: Otomatik deadlock tespiti

---

## Sonuç

Bu **Philosophers** projesi güncel haliyle:

1. **42 Norm Uyumlu**: pthread_exit kullanılmıyor
2. **Memory Safe**: Leak ve double-free yok
3. **Thread Safe**: Race condition koruması
4. **Deadlock Free**: Çoklu önleme stratejisi
5. **Optimized**: Timing ve senkronizasyon optimizasyonu
6. **Maintainable**: Temiz ve anlaşılır kod yapısı

Proje, sistem programlama ve concurrent programming konularında derinlemesine bilgi gerektirir ve modern thread-safe programlama prensiplerini gösterir.
