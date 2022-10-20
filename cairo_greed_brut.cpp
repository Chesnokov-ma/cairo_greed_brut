#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cmath>    

using namespace std;

#define N 40

class GREED_ALG
{
    private:
        int* spins;     // номера диполей для полного перебора среди них
        int spins_N;    // кол-во диполей для перебора
        int max_conf;   // кол-во конфигураций для перебора

        int* fixed_spins;           // номера фиксированных диполей
        int* fixed_spins_conf;      // значения фиксированных спинов (вниз или вверх)
        int fixed_spins_N;          // кол-во фиксированных диполей

        int p_mod;      // == 0 -> без фиксированных диполей

    public:
        //---------------------------------------
        //  Конструктор 1 : без фиксированных спинов  
        //---------------------------------------
        GREED_ALG(int spins_N, int spins[])
        {
            this->spins_N = spins_N;
            this->spins = spins;
            p_mod = 0;

        }

        //---------------------------------------
        //  Конструктор 2 : с фиксированными спинами  
        //---------------------------------------
        GREED_ALG(int spins_N, int spins[], int fixed_spins_N, int fixed_spins[], int fixed_spins_conf[]) : GREED_ALG(spins_N, spins)
        {
            p_mod = 1;
            this->fixed_spins_N = fixed_spins_N;
            this->fixed_spins = fixed_spins;
            this->fixed_spins_conf = fixed_spins_conf;
        }

        //---------------------------------------
        //  Сеттеры  
        //---------------------------------------
        void SetSpins(int spins_N, int spins[]) { this->spins_N = spins_N;  this->spins = spins; }  // Сеттер 1
        void SetSpinsAll(int spins_N, int spins[], int fixed_spins_N, int fixed_spins[], int fixed_spins_conf[]) // Сеттер 2
        {
            this->SetSpins(spins_N, spins);
            this->fixed_spins_N = fixed_spins_N;
            this->fixed_spins = fixed_spins;
            this->fixed_spins_conf = fixed_spins_conf;
        }


        //---------------------------------------
        //  ! Основная функция расчета 
        
        //  * show_all_min_conf == true --> показывать все конф с мин лок энергией
        //  * fast_calc == true --> расчет выше диагонали
        //  * print == true --> выводить на экран и в файл
        //---------------------------------------
        double Calculate(bool show_all_min_conf = true, bool fast_calc = true, bool print = true)
        {
            int fixed_count = 0;
            if (p_mod == 1)
            {
                for (int i = 0; i < spins_N; i++)
                {
                    for (int j = 0; j < fixed_spins_N; j++)
                    {
                        if (spins[i] == fixed_spins[j])
                        {fixed_count++;}
                    }
                }
            }

            max_conf = pow(2, spins_N - fixed_count);
            int spins_conf[max_conf][spins_N];
            int conf[max_conf][N];

            if (p_mod == 0)
            {
                //---------------------------------------
                //  Перебор среди конфигураций, заполнение массива spins_conf со всеми локальными конф  
                //---------------------------------------
            
                for (int i = 0; i < max_conf; i++)
                {
                    for (int j = 0; j < spins_N; j++)
                    {
                        if ((1 & i >> j) == 1)
                        {
                            spins_conf[i][j] = -1;
                        }
                        else if ((1 & i >> j) == 0)
                        {
                            spins_conf[i][j] = 1;
                        }
                    }
                }

                //---------------------------------------
                //  Генерация полных (с размером N) конфигураций
                //---------------------------------------
                for (int k = 0; k < max_conf; k++)
                {
                    int count = 0;
                    for (int i = 0; i < N; i++)
                    {
                        if (i == spins[count])
                        {
                            conf[k][i] = spins_conf[k][count];
                            count++;
                        }
                        else
                        {
                            conf[k][i] = 1;
                        }
                    }
                }
            }
            else if (p_mod == 1)        // при наличии фиксированных спинов
            {
                //---------------------------------------
                //  Перебор среди конфигураций, заполнение массива spins_conf со всеми локальными конф  
                //---------------------------------------
                for (int i = 0; i < max_conf; i++)
                {
                    for (int j = 0; j < spins_N - fixed_spins_N; j++)
                    {
                        if ((1 & i >> j) == 1)
                        {
                            spins_conf[i][j] = -1;
                        }
                        else if ((1 & i >> j) == 0)
                        {
                            spins_conf[i][j] = 1;
                        }
                    }
                }

                // for (int i = 0; i < max_conf; i++)
                // {
                //     for (int j = 0; j < spins_N - fixed_spins_N; j++)
                // 	    cout << spins_conf[i][j] << ", ";
                //     cout << endl;
                // }	


                //---------------------------------------
                //  Генерация полных (с размером N) конфигураций
                //---------------------------------------
                int fk = 0;
                for (int k = 0; k < max_conf; k++)
                {
                    fk = 0;
                    int count = 0, countloc = 0;
                    for (int i = 0; i < N; i++)
                    {
                        if (i == spins[count])
                        {
                            if (i == fixed_spins[fk])
                            {
                                if (fixed_spins_conf[fk] == 0)
                                {
                                    conf[k][i] = 1;
                                }
                                else 
                                {
                                    conf[k][i] = -1;
                                }
                                fk++;
                            }
                            else
                            {
                                conf[k][i] = spins_conf[k][countloc];
                                countloc++;
                            }
                            count++;
                        }
                        else
                        {
                            conf[k][i] = 1;
                        }
                    }
                }
            }
          
            // for (int i = 0; i < 10; i++)
            // {
            //     for (int j = 0; j < N; j++)
            // 	    cout << conf[i][j] << ", ";
            //     cout << endl;
            // }			
            
            
            //---------------------------------------
            //  Чтение массива парных энепгий 
            //---------------------------------------
            
            double matrixEn[N][N];
            FILE* em1 = fopen("em376_40.dat","r");   // чтение матрицы энергий
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                    fscanf(em1, "%lf ",&matrixEn[i][j]);
            }
            fclose(em1);

            double E = 0, E_loc = 0, E_min = 0, E_arr[max_conf];
            double E_glob = 0, E_glob_arr[max_conf];
            int E_min_conf_id = 0;

            int i_ind = 0, j_ind = 0;

            // ofstream Ef("Ef.dat");
            // ofstream El("El.dat");

            // conf для gs (em_40) из экселя для проверки
            // * 1, 0, 0,1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 0, 0,1,1,1
            // * 1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,-1,1,1,1

            // int tmp_arr[] = { 1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,-1,1,1,1 };
            // for (int i = 0; i < N; i++) conf[0][i] = tmp_arr[i];

            //---------------------------------------
            //  Поиск Emin и E_glob_min
            //---------------------------------------
            if (fast_calc)
            {
                for (int k = 0; k < max_conf; k++)
                {
                    E = 0;
                    i_ind = 0; j_ind = 0;

                    E_glob = 0;

                    for (int i = 0; i < N; i++)
                    {
                        E_loc = 0;
                        j_ind = i_ind + 1;

                        for (int j = i + 1; j < N; j++)
                        {
                            if (i == spins[i_ind] && j == spins[j_ind])
                            {
                                // cout << spins[i_ind] << " - " << spins[j_ind] << endl;
                                // cout << matrixEn[i][j] * conf[k][i] * conf[k][j] << endl;

                                E_loc += (matrixEn[i][j] * conf[k][i] * conf[k][j]);
                                j_ind++;
                            }

                            E_glob += (matrixEn[i][j] * conf[k][i] * conf[k][j]);
                            
                        }
                        // cout << i << ") E_loc = " << E_loc << endl;
                        E += E_loc;
                        
                        if (i == spins[i_ind])  i_ind++;
                    }
                    E;
                    E_glob;

                    E_arr[k] = E;
                    E_glob_arr[k] = E_glob;

                    cout << k << ") E_loc = " << E << "\t\t";     // вывод энергии конфигурацц
                    // for (int i = 0; i < N; i++)             // вывод конфигурации
                    // {
                    //     if (conf[k][i] == -1)
                    //         cout << "0";
                    //     else
                    //         cout << "1";
                    // }
                    cout << endl;

                    if (E < E_min)    { E_min = E; E_min_conf_id = k; }

                    // Ef << E_glob << ", ";
                    // El << E << ", ";
                }

                // Ef.close();
                // El.close();

            }
            else    // перебор всех элементов со всеми для проверки
            {
                // // for (int k = 0; k < 1; k++)
                // for (int k = 0; k < max_conf; k++)
                // {
                //     E = 0;
                //     i_ind = 0; j_ind = 0;

                //     E_glob = 0;

                //     for (int i = 0; i < N; i++)
                //     {
                //         E_loc = 0;
                //         j_ind = 0;

                //         for (int j = 0; j < N; j++)
                //         {
                //             if (i == spins[i_ind] && j == spins[j_ind])
                //             {
                //                 // cout << spins[i_ind] << " - " << spins[j_ind] << endl;
                //                 // cout << matrixEn[i][j] * conf[k][i] * conf[k][j] << endl;

                //                 E_loc += (matrixEn[i][j] * conf[k][i] * conf[k][j]);
                //                 j_ind++;
                //             }

                //             E_glob += (matrixEn[i][j] * conf[k][i] * conf[k][j]);
                            
                //         }
                //         // cout << i << ") E_loc = " << E_loc << endl;
                //         E += E_loc;
                        
                //         if (i == spins[i_ind])  i_ind++;
                //     }
                //     E /= 2;
                //     E_glob /= 2;

                //     E_arr[k] = E;
                //     E_glob_arr[k] = E_glob;

                //     cout << k << ") E = " << E << endl;     // вывод энергии конфигурацц

                //     if (E < E_min)    { E_min = E; E_min_conf_id = k; }

                //     Ef << E_glob << ", ";
                //     El << E << ", ";
                // }

                // Ef.close();
                // El.close();
            }

            //---------------------------------------
            //  Поиск конф с одинаковой энергией области
            //---------------------------------------
            if (show_all_min_conf)
            {
                int E_min_conf_count = 0;
                vector < vector <int> > E_min_conf;
                E_min_conf.clear();

                for (int i = 0; i < max_conf; i++)
                {
                    if (E_arr[i] == E_min)
                    {
                        vector <int> tmp;
                        tmp.clear();
                        for (int j = 0; j < N; j++)
                        {
                            tmp.push_back(conf[i][j]);
                        }
                        E_min_conf.push_back(tmp);

                        E_min_conf_count++;
                    }
                }

                cout <<"\nКонфигурации с мин локальной E: \n";
                for (int k = 0; k < E_min_conf_count; k++)
                {
                    double Ek = 0;
                    for (int d = 0; d < N; d++)
                        if (E_min_conf[k][d] == -1)
                            cout << "0";
                        else
                            cout << "1";

                    for (int i = 0; i < N; i++)
                    {
                        for (int j = 0; j < N; j++)
                        {
                            Ek += (matrixEn[i][j] * E_min_conf[k][i] * E_min_conf[k][j]);
                            j_ind++;
                        }   
                    }

                    cout << " => E_loc = " << E_min << ", E_glob = " << (Ek / 2) << endl;
                }
            }
            

            //---------------------------------------
            //  Вывод на экран и в файл 
            //---------------------------------------
            if (print)
            {
                ofstream fmin("E_min_40.dat");
                cout << "\nEmin_loc = " << E_min << "\nE_glob = " << E_glob_arr[E_min_conf_id] << "\ngs = -0.17995"
                << "\nномер = " << E_min_conf_id << "\nлокальная конф: ";
                
                fmin << "Emin_loc = " << E_min << "\nE_glob = " << E_glob_arr[E_min_conf_id] << "\ngs = -0.17995"
                << "\nномер = " << E_min_conf_id << "\nлокальная конф: ";

                for (int i = 0; i < spins_N; i++) 
                {
                    if (spins_conf[E_min_conf_id][i] == -1) { cout << 0; fmin << 0; }
                    else { cout << 1; fmin << 1; }
                }
                cout << "\nполная конф: ";
                fmin << "\nполная конф: ";

                for (int i = 0; i < N; i++) 
                {
                    if (conf[E_min_conf_id][i] == -1) { cout << 0; fmin << 0; }
                    else { cout << 1; fmin << 1; }
                }
                cout << endl;
                fmin << endl;

                for (int i = 0; i < spins_N; i++) {if (i != N - 1) fmin << spins[i] << ", ";   else fmin << spins[i]; }
                for (int i = 0; i < N; i++) fmin << endl << conf[E_min_conf_id][i]; cout << endl;

                fmin.close();
            }

            return E_min;



            // return 0;
        }

        
};






int main()
{
    //---------------------------------------
    //  Пример заполнения входных данных
    //---------------------------------------

    int spins[] = {1, 2, 6, 9, 11, 12, 16, 19, 22, 23, 28, 29, 32, 33, 38, 39};     // всегда должен быть отсортирован по возростанию
    int spins_N = sizeof(spins)/sizeof(*spins);

    int fixed_spins[] = {1, 2, 39};         // только диполи из spins            // всегда должен быть отсортирован по возростанию
    int fixed_spins_conf[] = {1, 1, 0};                                           // идут в том же порядке
    int fixed_spins_N = sizeof(fixed_spins)/sizeof(*fixed_spins);

    double E = 0;
    


    GREED_ALG g_a (spins_N, spins);
    // GREED_ALG g_a(spins_N, spins, fixed_spins_N, fixed_spins, fixed_spins_conf);        // с фикс спинами
    E = g_a.Calculate();






    return 0;
}