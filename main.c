#include <stdio.h>



typedef struct s_philo
{
   int right_fork;
   int left_fork;

   
   // mutex fonksiyonları gerekiyor 

}  t_philo;







int main(int ac, char **av)
{

   //struct yapısı yapılacak( forglar için birer tane sağ sol ve forgları handle ettiğinde forkların elde olduğu durumlarda mutex e gönderiliecek)
   if (ac == 4 ) // ./philo 4(filozof sayısı) 401(geçirilen süre) 200(yemek) 200 (uyku) 5(yemek yeme sayısı program sonlanması için)
   {
    
   }
   else if (ac == 5)
   {
    /* code */
   }
   else
   {

   }
}