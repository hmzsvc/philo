# Philosophers Projesi - Detaylı Kod Analizi

## 📖 İçindekiler
1. [Proje Genel Bakış](#proje-genel-bakış)
2. [Dosya Yapısı](#dosya-yapısı)
3. [Veri Yapıları](#veri-yapıları)
4. [Program Akışı](#program-akışı)
5. [Fonksiyon Analizleri](#fonksiyon-analizleri)
6. [Thread Yönetimi](#thread-yönetimi)
7. [Mutex Kullanımı](#mutex-kullanımı)
8. [Hata Yönetimi](#hata-yönetimi)

---

## Proje Genel Bakış

**Philosophers** projesi, klasik "Dining Philosophers" problemini çözen bir C programıdır. Bu problem, eşzamanlılık (concurrency) ve deadlock önleme konularını öğretmek için kullanılan ünlü bir bilgisayar bilimi problemidir.

### Problem Tanımı:
- N adet filozof bir masanın etrafında oturuyor
- Her filozofun yanında birer çatal var
- Filozoflar sadece 3 şey yapıyor: yemek yeme, uyuma, düşünme
- Yemek yemek için 2 çatala ihtiyaç var (sol ve sağ)
- Filozoflar ölmemeli (açlıktan)

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
│   ├── mutex_utils.c        # Mutex yönetimi
│   ├── philo_routine.c      # Filozof rutinleri
│   ├── philo_utils.c        # Filozof yardımcıları
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
    int                id;                  // Filozofun kimliği (1, 2, 3...)
    int                left_fork_bool;      // Sol çatal kullanımda mı?
    int                right_fork_bool;     // Sağ çatal kullanımda mı?
    int                eat_count;           // Kaç kez yemek yedi
    long long          last_meal_time;      // Son yemek zamanı
    pthread_t          thread;              // Filozofun thread'i
    pthread_mutex_t    *left_fork;          // Sol çatal mutex'i
    pthread_mutex_t    *right_fork;         // Sağ çatal mutex'i
    pthread_mutex_t    eat_count_mutex;     // Yemek sayısı mutex'i
    t_data             *data;               // Ana veri yapısına pointer
} t_philo;
```

### 2. `t_data` Yapısı (Oyun Durumu)
```c
typedef struct s_data
{
    int                philo_count;         // Filozof sayısı
    int                dead_index;          // Ölen filozofun index'i
    int                must_eat;            // Zorunlu yemek sayısı
    int                is_dead;             // Ölüm durumu (0,1,2,3)
    int                time_to_die;         // Ölüm süresi (ms)
    int                time_to_eat;         // Yemek süresi (ms)
    int                time_to_sleep;       // Uyku süresi (ms)
    int                start_flag;          // Başlama sinyali
    int                must_meal_num;       // Yemek hedefine ulaşan sayısı
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
} t_data;
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
    t_data data;

    if (argc == 5 || argc == 6)  // Argüman kontrolü
    {
        init_philo(&data, argv, argc);      // 1. Veri yapısını başlat
        init_forks(&data);                  // 2. Mutex'leri başlat
        monitor_philo_create(&data);        // 3. İzleyici thread'i oluştur
        create_philo(&data);                // 4. Filozof thread'lerini oluştur
        philo_join(&data);                  // 5. Thread'leri bekle
        cleanup(&data);                     // 6. Temizlik yap
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

### 📂 src/philo.c

#### `main` Fonksiyonu
```c
int main(int argc, char *argv[])
```
- **Amaç**: Programın giriş noktası
- **Akış**: 
  1. Argüman sayısını kontrol et (5 veya 6 olmalı)
  2. Sırayla başlatma fonksiyonlarını çağır
  3. Thread'leri bekle ve temizlik yap

#### `ft_usleep` Fonksiyonu
```c
void ft_usleep(int wait_time, t_philo *philo)
```
- **Amaç**: Kesin süre bekleme (normal usleep'ten daha hassas)
- **Çalışma**: 
  1. Başlangıç zamanını al
  2. Hedef süre kadar döngüde bekle
  3. Her 100 mikrosaniyede ölüm kontrolü yap

### 📂 utils/utils.c

#### `ft_atoi` Fonksiyonu
```c
int ft_atoi(char *str, int *res)
```
- **Amaç**: String'i integer'a çevir (güvenli versiyon)
- **Kontroller**:
  1. Uzunluk kontrolü (11 karakterden az)
  2. Negatif sayı kontrolü (hata)
  3. Karakter geçerliliği kontrolü
  4. Integer overflow kontrolü

#### `parse_args` Fonksiyonu
```c
void parse_args(char *argv[], t_data *data, int argc)
```
- **Amaç**: Komut satırı argümanlarını parse et
- **Argümanlar**:
  1. `argv[1]`: Filozof sayısı
  2. `argv[2]`: Ölüm süresi (ms)
  3. `argv[3]`: Yemek süresi (ms)
  4. `argv[4]`: Uyku süresi (ms)
  5. `argv[5]`: (Opsiyonel) Zorunlu yemek sayısı

#### `print` Fonksiyonu
```c
void print(t_philo *philo, char *str)
```
- **Amaç**: Thread-safe yazdırma
- **Akış**:
  1. Yemek hedefi kontrolü yap
  2. Ölüm kontrolü yap
  3. Print mutex'i kilitle
  4. Ölü değilse mesaj yazdır
  5. Mutex'i aç

### 📂 utils/philo_utils.c

#### `init_philo` Fonksiyonu
```c
void init_philo(t_data *data, char *argv[], int argc)
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
void create_philo(t_data *data)
```
- **Amaç**: Filozof thread'lerini oluştur
- **Akış**:
  1. Her filozof için thread oluştur
  2. Thread oluşturma hatası varsa hata flag'i set et
  3. Başlangıç zamanını kaydet
  4. Başlama sinyali ver

#### `init_forks` Fonksiyonu
```c
void init_forks(t_data *data)
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

#### `philo_process` Fonksiyonu (static)
```c
static void *philo_process(void *arg)
```
- **Amaç**: Her filozofun ana rutini
- **Akış**:
  1. Başlama sinyalini bekle
  2. Tek filozof özel durumunu kontrol et
  3. Senkronizasyon yap
  4. Ana döngü:
     - Çatalları al
     - Yemek ye
     - Çatalları bırak
     - Uyu
     - Düşün
  5. Ölüm durumunda çık

### 📂 utils/philo_routine.c

#### `philo_eat` Fonksiyonu
```c
void philo_eat(t_philo *philo)
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
```
- **Amaç**: Filozof uyur
- **Adımlar**:
  1. "is sleeping" mesajını yazdır
  2. Uyku süresince bekle

#### `philo_thinking` Fonksiyonu
```c
void philo_thinking(t_philo *philo)
```
- **Amaç**: Filozof düşünür
- **Hesaplama**: `time_to_die - (time_to_eat + time_to_sleep)` kadar bekle
- **Amaç**: Optimal timing için

#### `philo_dead` Fonksiyonu
```c
void philo_dead(t_philo philo)
```
- **Amaç**: Ölüm mesajını yazdır
- **Format**: `[zaman] [id] died`

#### `philo_take_fork` Fonksiyonu
```c
void philo_take_fork(t_philo *philo)
```
- **Amaç**: Çatalları al (deadlock önleyici)
- **Strateji**:
  - **Çift ID**: Sol sonra sağ çatalı al
  - **Tek ID**: Sağ sonra sol çatalı al (20µs gecikme ile)
- **Deadlock Önleme**: Farklı sıralama ile

### 📂 utils/mutex_utils.c

#### `check_dead` Fonksiyonu
```c
int check_dead(t_philo *philo)
```
- **Amaç**: Ölüm durumunu kontrol et
- **Döndürür**: 1 ölü ise, 0 yaşıyor ise
- **Durumlar**:
  - `is_dead = 1`: Normal ölüm
  - `is_dead = 2`: Thread hatası
  - `is_dead = 3`: Herkes yeterince yedi

#### `check_start_flag` Fonksiyonu
```c
int check_start_flag(t_philo *philo)
```
- **Amaç**: Başlama sinyalini kontrol et
- **Kullanım**: Thread'lerin senkronize başlaması için

#### `handle_dead` Fonksiyonu
```c
void handle_dead(t_philo *philo)
```
- **Amaç**: Ölüm durumunda temizlik
- **Akış**:
  1. Ölüm durumunu kontrol et
  2. Tutulan çatalları bırak
  3. Thread'den çık

#### `check_meal_goal` Fonksiyonu
```c
void check_meal_goal(t_philo *philo)
```
- **Amaç**: Yemek hedefini kontrol et
- **Akış**:
  1. Hedef yemek sayısına ulaştı mı kontrol et
  2. Ulaştıysa global sayacı artır
  3. Çatalları bırak ve thread'den çık

#### `last_meal_added` Fonksiyonu
```c
void last_meal_added(t_philo *philo)
```
- **Amaç**: Son yemek zamanını güncelle (thread-safe)

### 📂 utils/monitor.c

#### `monitor_process` Fonksiyonu (static)
```c
static void *monitor_process(void *argv)
```
- **Amaç**: Filozofları sürekli izle
- **Akış**:
  1. Başlama sinyalini bekle
  2. Sonsuz döngüde:
     - Her filozofu kontrol et
     - Ölüm zamanını kontrol et
     - Yemek hedefini kontrol et
  3. Problem varsa çık

#### `monitor_philo_create` Fonksiyonu
```c
void monitor_philo_create(t_data *data)
```
- **Amaç**: İzleyici thread'ini oluştur
- **Hata Durumu**: Thread oluşturulamazsa hata flag'leri set et

### 📂 utils/monitor_utils.c

#### `set_death_status` Fonksiyonu (static)
```c
static void set_death_status(t_data *data, int philo_index)
```
- **Amaç**: Ölüm durumunu thread-safe şekilde set et

#### `check_and_handle_death` Fonksiyonu
```c
void check_and_handle_death(t_data *data, int philo_index)
```
- **Amaç**: Belirli filozofun ölüm durumunu kontrol et
- **Kontrol**: `current_time - last_meal_time >= time_to_die`

#### `wait_start` Fonksiyonu
```c
void wait_start(t_data *data)
```
- **Amaç**: Başlama sinyalini bekle
- **Kullanım**: İzleyici thread için

### 📂 utils/helpers.c

#### `philo_join` Fonksiyonu
```c
void philo_join(t_data *data)
```
- **Amaç**: Bütün thread'leri bekle
- **Sıra**:
  1. İzleyici thread'ini bekle
  2. Ölüm durumunda ölüm mesajını yazdır
  3. Bütün filozof thread'lerini bekle

#### `ft_calloc` Fonksiyonu
```c
void *ft_calloc(size_t count, size_t size)
```
- **Amaç**: Bellek tahsis et ve sıfırla
- **Güvenlik**: NULL kontrolü var

#### `get_time_in_ms` Fonksiyonu
```c
long long get_time_in_ms(void)
```
- **Amaç**: Millisaniye cinsinden zaman al
- **Kullanım**: `gettimeofday` kullanarak

#### `set_time` Fonksiyonu
```c
void set_time(t_data *data)
```
- **Amaç**: Başlangıç zamanını set et

### 📂 error/error_handle.c

#### `cleanup` Fonksiyonu
```c
void cleanup(t_data *data)
```
- **Amaç**: Bütün kaynakları temizle
- **Temizlik**:
  1. Bütün mutex'leri destroy et
  2. Bellek alanlarını free et

#### `error_check` Fonksiyonu
```c
void error_check(t_data *data, int err_code, void *ptr)
```
- **Amaç**: Hata durumlarını kontrol et
- **Akış**:
  1. Pointer NULL ise hata
  2. Hata mesajını yazdır
  3. Temizlik yap
  4. Program sonlandır

#### `error_check_mutex` Fonksiyonu
```c
void error_check_mutex(t_data *data, int value)
```
- **Amaç**: Mutex işlem hatalarını kontrol et
- **Kontrol**: `value != 0` ise hata

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
- İlk oluşturulan thread

#### Filozof Thread'leri (philo_process)
- Her filozof için bir thread
- Ana yaşam döngüsünü işletir
- Çatal rekabeti yapar

### 2. Thread Senkronizasyonu

```c
// Başlama senkronizasyonu
void sync_philo_start(t_philo *philo)
{
    // Son yemek zamanını set et
    pthread_mutex_lock(&philo->data->meal_mutex);
    philo->last_meal_time = get_time_in_ms();
    pthread_mutex_unlock(&philo->data->meal_mutex);
    
    // Tek ID'li filozoflar gecikme ile başlar
    if (philo->id % 2 != 0)
        ft_usleep(philo->data->time_to_eat, philo);
}
```

### 3. Thread Yaşam Döngüsü

1. **Oluşturma**: `pthread_create`
2. **Başlama Bekleme**: `start_flag` sinyali
3. **Senkronizasyon**: Çift/tek filozof ayrımı
4. **Ana Döngü**: Yeme-uyuma-düşünme
5. **Sonlanma**: Ölüm veya hedef yemek
6. **Bekleme**: `pthread_join`

---

## Mutex Kullanımı

### 1. Çatal Mutex'leri (`forks`)
```c
pthread_mutex_t *forks; // Çatal dizisi
```
- **Amaç**: Her çatalın tekil kullanımını garanti et
- **Kullanım**: Yemek yerken 2 çatal gerekli
- **Deadlock Önleme**: Farklı alma sırası

### 2. Ölüm Mutex'i (`death_mutex`)
```c
pthread_mutex_t death_mutex;
```
- **Koruduğu Değişkenler**: `is_dead`, `dead_index`
- **Kullanım**: Ölüm durumu kontrolü ve set etme

### 3. Yazdırma Mutex'i (`print_mutex`)
```c
pthread_mutex_t print_mutex;
```
- **Amaç**: Mesajların karışmaması
- **Garanti**: Bir seferde bir mesaj

### 4. Başlama Mutex'i (`start_flag_mutex`)
```c
pthread_mutex_t start_flag_mutex;
```
- **Amaç**: Thread'lerin senkronize başlaması
- **Kullanım**: `start_flag` değişkenini korur

### 5. Yemek Mutex'leri
```c
pthread_mutex_t check_meal_mutex;  // Yemek kontrolü
pthread_mutex_t meal_mutex;        // Yemek zamanı
pthread_mutex_t must_meal_mutex;   // Zorunlu yemek sayacı
```

### 6. Bireysel Filozof Mutex'i
```c
pthread_mutex_t eat_count_mutex; // Her filozofun yemek sayısı
```

---

## Hata Yönetimi

### 1. Hata Türleri

#### Bellek Hatası (`ERR_MALLOC_FAIL`)
```c
data->philos = ft_calloc(sizeof(t_philo), data->philo_count);
error_check(data, ERR_MALLOC_FAIL, data->philos);
```

#### Argüman Hatası (`ERR_INVALID_ARG`)
```c
if (argc != 5 && argc != 6)
    printf("ERR_INVALID_ARG\n");
```

#### Thread Hatası (`ERR_THREAD_FAIL`)
```c
if (pthread_create(&thread, NULL, function, arg))
    error_check(data, ERR_THREAD_FAIL, NULL);
```

#### Parsing Hatası (`ERR_ATOI_FAIL`)
```c
if (!(*(str + i) >= '0' && *(str + i) <= '9'))
    error_check(NULL, ERR_ATOI_FAIL, NULL);
```

### 2. Hata İşleme Stratejisi

1. **Immediate Cleanup**: Hata anında temizlik
2. **Resource Release**: Mutex ve bellek temizliği
3. **Graceful Exit**: Program düzgün sonlanma

---

## Özel Durumlar

### 1. Tek Filozof Durumu
```c
static void one_philo_handle(t_philo *philo)
{
    if (philo->data->philo_count == 1)
    {
        pthread_mutex_lock(philo->left_fork);
        print(philo, "has taken a fork");
        pthread_mutex_unlock(philo->left_fork);
        // Ölüm flag'i set et
        pthread_mutex_lock(&philo->data->death_mutex);
        philo->data->is_dead = 1;
        pthread_mutex_unlock(&philo->data->death_mutex);
    }
}
```

### 2. Deadlock Önleme
```c
// Çift ID: Sol-Sağ sırası
if (philo->id % 2 == 0)
{
    pthread_mutex_lock(philo->left_fork);
    pthread_mutex_lock(philo->right_fork);
}
// Tek ID: Sağ-Sol sırası (gecikme ile)
else
{
    pthread_mutex_lock(philo->right_fork);
    usleep(20);
    pthread_mutex_lock(philo->left_fork);
}
```

### 3. Timing Optimizasyonu
```c
// Düşünme süresi hesaplama
usleep(philo->data->time_to_die - 
       (philo->data->time_to_eat + philo->data->time_to_sleep));
```

---

## Sonuç

Bu **Philosophers** projesi karmaşık bir concurrent programming örneğidir. Ana öğeleri:

1. **Thread Management**: Birden fazla thread'in koordinasyonu
2. **Mutex Usage**: Race condition'ları önleme
3. **Deadlock Prevention**: Farklı kilitleme sıraları
4. **Resource Sharing**: Çatal paylaşımı
5. **Timing Control**: Hassas zaman kontrolü
6. **Error Handling**: Kapsamlı hata yönetimi

Proje, sistem programlama ve concurrent programming konularında derinlemesine bilgi gerektirir ve thread-safe programlama prensiplerini gösterir.
