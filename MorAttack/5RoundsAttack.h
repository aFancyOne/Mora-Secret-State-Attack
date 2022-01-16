// Атака против 5 раундов

class FiveRoundsAttack{

private:

    bool dX4_isFound = false;
    std::array<uint64_t, 15> dX4{};
    std::array<uint64_t, 15> dX3{};
    uint64_t Z5 = 0x0000000000000000LL;
    uint64_t operationsCounter5 = 0; // Переменная для подсчёта количества операций в атаке на 5 раундов (временная сложность)


    // Формирование 1-го столбца полубайтов. Используется при угадывании 16 битов в Y3.
    uint64_t firstColumnFormation(short  i, short  j, short  w, short  z) {
        uint64_t column;
        column = i;
        column <<= 16;
        column += j;
        column <<= 16;
        column += w;
        column <<= 16;
        column += z;
        column <<= 12;
        return column;
    }

    /* Формирование 60 - битного значения из первых 4 битов(полубайта) каждой из 15 "разностей" в d-"разности" dX4
     * для хранения в векторе backward */
    uint64_t firstHalfBytesFormation(const std::array<uint64_t, 15>& dDiff) {
        uint64_t firstHalfBytes = 0;
        for (int i = 0; i < 15; i++) {
            firstHalfBytes <<= 4;
            firstHalfBytes += dDiff[i] >> 60;
        }
        return firstHalfBytes;
    }

    // Метод аналогичен предыдущему, но формирует 60-битное значение из 4 битов num полубайта, где num = [2,3,4]
    uint64_t halfBytesFormation(const std::array<uint64_t, 15>& dDiff, uint8_t num) {
        uint64_t HalfBytes = 0;
        for (int i = 0; i < 15; i++) {
            HalfBytes <<= 4;
            HalfBytes += ((dDiff[i] << (4*(num - 1))) >> 60);
        }
        return HalfBytes;
    }

    // Метод используется для сортировки вектора backward. Сортировка происходит по возрастанию для всех элементов backward[0]
    void vectorSort(std::vector<std::array<uint64_t, 2>>& backward) {
        std::cout << "Vector sorting has started " << std::endl;
        unsigned int sort_start_time = clock();
        std::make_heap(backward.begin(), backward.end());
        std::sort_heap(backward.begin(), backward.end());
        operationsCounter5 += 20*pow(2,20); // Сложность сортировки
        std::cout << "Vector was sorted in " << ((clock() - sort_start_time) / (double)CLOCKS_PER_SEC) << " seconds" << std::endl;
    }


    /* Двоичный поиск. Метод ищет совпадения первых полубайтов "разностей", полученных в прямых преобразованиях,
     * среди элементов вектора backward. Сложность двоичного поиска: O(log n), где n - кол-во элементов вектора.
     * Кол-во элементов в backward на момент вызова метода = 2^20   ==>   в цикле метода выполнится не более 20 итераций */
    bool binarySearch(const std::vector<std::array<uint64_t, 2>>& backward, uint64_t dX4_forward) {
        int left = 0;
        int right = backward.size() - 1;
        int mid;
        while (left <= right) {
            operationsCounter5++; // Сложность одной операции сравнения (20 операций на один элемент вектора forward)
            mid = (left + right) / 2;
            if (backward[mid][0] == dX4_forward) {
                Z5 += backward[mid][1]; // Если найдётся совпадение, запоминаются 16 правильно угаданных битов Z5
                Z5 <<= 48;
                return true;
            }
            else if (backward[mid][0] > dX4_forward) right = mid - 1;
            else left = mid + 1;
        }
        return false;
    }

    /* Метод, использующийся в прямых преобразованиях для сравнения получившихся
     * d-"разностей" dX4 с элементами вектора обратных преобразований */
    bool forwardEqualityCheckUp(const std::vector<std::array<uint64_t, 15>>& forward,
                                const std::vector<std::array<uint64_t, 2>>& backward, uint8_t i, uint8_t j) {

        if (j > 0xf) return false;
        uint64_t x;
        std::array<uint64_t, 15> dCurrent;
        // Метод используется во вложенном цикле - 2 цикла по 16 итераций в каждом
        for (uint8_t w = 0; w <= 0xf; w++) {                          // 16 итераций
            for (uint8_t z = 0; z <= 0xf; z++) {                      // 16 итераций
                if (dX4_isFound) return false;
                x = firstColumnFormation(i, j, w, z);
                for (uint32_t r = 0; r < forward.size(); r++) {       // 65 536 итераций
                    dCurrent = dL(dP(dS(forward.at(r), x)));
                    operationsCounter5 += 15; // Сложность одного (из 2^32) элемента вектора forward (прямые преобразования)
                    /* Преобразования
                     * S: dY3 -> dZ3
                     * P: dZ3 -> dW3
                     * L: dW3 -> dX4
                     *
                     * Сравниваем результаты по первому полубайту с вариантами dX4 из backward,
                     * не занося их в вектор forward
                     *
                     * В двоичном поиске 20 итераций  */
                    if (binarySearch(backward, firstHalfBytesFormation(dCurrent))) {
                        dX3 = forward.at(r);
                        dX4 = dCurrent;
                        dX4_isFound = true;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /*
     * -------------------------- Прямые преобразования --------------------------
     * 
     */

    bool forwardPropagations(std::array<uint64_t, 15> dM, uint64_t M0,
           const std::vector<std::array<uint64_t, 2>>& backward) {

        std::cout << "Forward propagations have started, first round key = 0000000000000000" << std::endl;
        unsigned int FP_start_time = clock();

        /* Вектор с d-"разностями" прямых преобразований. В нём будет храниться 2^16 значений Y2.
         * Последующие значения будут сравниваться напрямую, т.к. в их хранении нет смысла */
        std::vector<std::array<uint64_t, 15>> forward;

        std::array<uint64_t, 15> dCurrent; // Текущая d-"разность"

        uint64_t firstRoundKey = 0x0000000000000000LL;

        do {      // 16 итераций

            /* x для первого S-преобразования (Y1) находится через XOR (X-преобразование) входного текста
             * и ключа первого раунда, полубайт которого мы угадываем */
            uint64_t x = M0 ^ firstRoundKey;

            // Преобразования d-"разностей" 1-го раунда
            dCurrent = dL(dP(dS(dM, x)));
            /* Преобразования:
             * S: dY1 -> dZ1
             * P: dZ1 -> dW1
             * L: dW1 -> dX2
             *
             *
             * Преобразования d-"разностей" 2-го раунда
             * 
             * S-преобразование dY2 -> dZ2 */
            std::cout << "    Guessing 16 bits (row) of Y2: ";
            for (uint32_t i = 0; i <= 0xffff; i++) {
                x = i;
                x <<= 48;
                forward.insert(forward.end(), dS(dCurrent, x));
            }
            std::cout << "Done." << std::endl;
            /* После этого S-преобразования в векторе будет храниться 65536 d-"разностей"
             *
             * Все дальнейшие преобразования будут производиться для каждой d-"разности" из вектора отдельно  */
            for (uint32_t i = 0; i < forward.size(); i++) {
                forward.at(i) = dP(forward.at(i));  // P: dZ2 -> dW2
                forward.at(i) = dL(forward.at(i));  // L: dW2 -> dX3
            }


            /* Преобразования d-"разностей" 3-го раунда
             * S-преобразование dY3 -> dZ3   */
            std::cout << "    Guessing 16 bits (column) of Y3: ";
            
            unsigned int FPw_start_time = clock();
            
            /* Потоки обрабатывают 2^16 * 2^16 * 20 итераций сравнения dX4 в прямых и обратных преобразованиях.
             * Поиск идёт по первому полубайту "разностей" элементов вектора backward. Для поиска 
             * используется 6 параллельных потоков для первых 12 итераций цикла с переменной j и 4 потока для последних 4-х  */
            for (uint8_t i = 0; i <= 0xf; i++) {
                for (uint8_t j = 0; j <= 0xf; j += 6) {
                    std::thread t_zero(&FiveRoundsAttack::forwardEqualityCheckUp, this, std::ref(forward), std::ref(backward), i, j);
                    std::thread t_one(&FiveRoundsAttack::forwardEqualityCheckUp, this, std::ref(forward), std::ref(backward), i, j + 1);
                    std::thread t_two(&FiveRoundsAttack::forwardEqualityCheckUp, this, std::ref(forward), std::ref(backward), i, j + 2);
                    std::thread t_three(&FiveRoundsAttack::forwardEqualityCheckUp, this, std::ref(forward), std::ref(backward), i, j + 3);
                    std::thread t_four(&FiveRoundsAttack::forwardEqualityCheckUp, this, std::ref(forward), std::ref(backward), i, j + 4);
                    std::thread t_five(&FiveRoundsAttack::forwardEqualityCheckUp, this, std::ref(forward), std::ref(backward), i, j + 5);
                    t_zero.join();
                    t_one.join();
                    t_two.join();
                    t_three.join();
                    t_four.join();
                    t_five.join();
                    if (dX4_isFound) {
                        std::cout << "Done." << std::endl;
                        std::cout << "Forward propagations finished in " << ((clock() - FP_start_time) / (double)CLOCKS_PER_SEC) / 60 << " minutes" << std::endl;
                        return true;
                    }
                }
            }

            // Если не найдено совпадений, то ключ меняется, и весь цикл начинается заново 
            forward.clear();
            firstRoundKey += 0x1000000000000000LL; 
            if (firstRoundKey != 0) std::cout << "    Done, but no match was found. New first round key = " << firstRoundKey << std::endl;
        } while (firstRoundKey != 0);
          
        std::cout << "    dX4 wasn't found" << std::endl;
        std::cout << "Forward propagations finished in " << ((clock() - FP_start_time) / (double)CLOCKS_PER_SEC) / 60 << " minutes" << std::endl;
        return false;

    }

    /*
     *  -------------------------- Обратные преобразования --------------------------
     *
     */
    std::vector<std::array<uint64_t, 2>> backwardPropagations(const std::array<uint64_t, 15>& dR, int columnNum) {

        if (columnNum == 1) std::cout << "Backward propagations have started " << std::endl;
        unsigned int BP_start_time = clock();

        // Будет использоваться для временного хранения 65 536 вариантов dY5
        std::vector<std::array<uint64_t, 15>> backwardBuff;

        /* Для экономии вместо 15-мерных d-"разностей" в backward будут заноситься по два элемента:
         * 1) Столбец первых полубайтов всех 15 "разниц" каждой d-"разности" (60 бит) - backward[0]
         * 2) Угаданные биты Z5, соответствующие d-"разности" (16 бит)                - backward[1] */
        std::vector<std::array<uint64_t, 2>> backward;

        std::array<uint64_t, 15> dCurrent;
        std::array<uint64_t, 2> dCurr;

        uint64_t y;

        dCurrent = dP(dL_Inverted(dR)); // Обратные L и P преобразования dR -> dZ5


        // S-преобразование dZ5 -> dY5
        if (columnNum == 1) std::cout << "    Guessing 16 bits (row) of Z5: ";
        for (uint32_t i = 0; i <= 0xffff; i++) {
            y = i;
            y <<= (64 - 16 * columnNum);
            backwardBuff.insert(backwardBuff.end(), dS_Inverted(dCurrent, y));
        }
        if (columnNum == 1) std::cout << "Done." << std::endl;

        /* После этого S-преобразования в векторе будет храниться 65536 d-"разностей"
         * Все дальнейшие преобразования будут производиться для каждой d-"разности" из вектора отдельно  */

        // Обратные L и P преобразования dX5 -> dZ4
        for (uint32_t i = 0; i < backwardBuff.size(); i++) {
            backwardBuff.at(i) = dL_Inverted(backwardBuff.at(i));
            backwardBuff.at(i) = dP(backwardBuff.at(i));
        }

        // S-преобразование dZ4 -> dY4
        if (columnNum == 1) std::cout << "    Guessing 4 bits of Z4: ";
        for (uint8_t i = 0; i <= 0xf; i++) {
            y = i;
            y <<= (64 - 4 * columnNum);

            /* При первом прохождении генерируется 1 048 576 вариантов dX4, в которых известен
             * первый полубайт. С ними будет сравниваться dX4 в прямых преобразованиях. */
            if (columnNum == 1) {
                for (uint32_t j = 0; j < backwardBuff.size(); j++) {
                    dCurr[0] = firstHalfBytesFormation(dS_Inverted(backwardBuff.at(j), y));
                    dCurr[1] = j;
                    backward.insert(backward.end(), dCurr);
                    operationsCounter5 += 15; // Сложность одного (из 2^20) элемента вектора backward (обратные преобразования)
                }
            }
            /* При последующих 3-х прохождениях будет происходить простое сравнение 2, 3 и 4 полубайтов 
             * с соответствующими полубайтами X4, который к этому моменту уже будет известен. Правильно
             * угаданные биты сразу заносятся в Z5. */
            else {
                uint64_t dX4c = halfBytesFormation(dX4, columnNum);
                for (uint32_t j = 0; j < backwardBuff.size(); j++) {
                    operationsCounter5 += 15; // Сложность одного (из 3*2^20) варианта угадываемого полубайта Z5
                    if (dX4c == halfBytesFormation(dS_Inverted(backwardBuff.at(j), y), columnNum)) {
                        y = j;
                        y <<= (64 - 16 * columnNum);
                        Z5 += y;
                        std::cout << "    Guessing of " << columnNum << " row of Z5 was successful" << std::endl;
                        return backward;
                    }
                }
            }
        }
        if (columnNum == 1) {
            std::cout << "Done." << std::endl;
            std::cout << "Backward propagations finished in " << (clock() - BP_start_time) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
        }
        else std::cout << "    Guessing of " << columnNum << " row of Z5 has failed" << std::endl;

        return backward;
    }

    // Угадываение Z5
    void Z5Guessing(std::array<uint64_t, 15> dR) {

        std::cout << "Guessing of Z5 has started " << std::endl;
        unsigned int Z5G_start_time = clock();
        /* Первая строчка была найдена при первом прохождении обратных преобразований
         * Три оставшиеся строчки угадываются параллельно */
        std::thread t_secondRow(&FiveRoundsAttack::backwardPropagations, this, std::ref(dR), 2);
        std::thread t_thirdRow(&FiveRoundsAttack::backwardPropagations, this, std::ref(dR), 3);
        std::thread t_fourthRow(&FiveRoundsAttack::backwardPropagations, this, std::ref(dR), 4);
        t_secondRow.join();
        t_thirdRow.join();
        t_fourthRow.join();
        std::cout << "Guessing of Z5 finished in " << (clock() - Z5G_start_time) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
    }


public:

    FiveRoundsAttack() {}

    uint64_t start(const std::array<uint64_t, 16>& M, const std::array<uint64_t, 16>& R5, const std::array<uint64_t, 16>& R4) {

        std::cout << "Attack has started " << std::endl;

        unsigned int start_time = clock();

        // Поиск 15 d-"разниц" dM
        std::array<uint64_t, 15> dM;
        for (int i = 1; i < 16; i++) {
            dM[i - 1] = M[0] ^ M[i];
        }
        // Поиск 15 d-"разниц" dR
        std::array<uint64_t, 15> dR;
        for (int i = 1; i < 16; i++) {
            dR[i - 1] = R5[0] ^ R5[i];
        }

        // Обратные преобразования. Находим 1 048 576 вариантов dX4 с известным первым полубайтом
        std::vector<std::array<uint64_t, 2>> backward = backwardPropagations(dR, 1);

        // Сортировка элементов вектора для возможности использования метода двоичного поиска
        vectorSort(backward);

        /* Прямые преобразования. Ищем среди 4 294 967 296 вариантов dX4 совпадение по первому полубайту
         * с одним из 1 048 576 вариантов dX4 из вектора backward (в памяти сохраняется только один найденный dX4) */
        forwardPropagations(dM, M[0], backward);

        for (int h = 0; h < 15; h++) {
            std::cout << "  dX4[" << h << "] = " << dX4[h] << std::endl;
        }

        // Угадывание оставшихся строчек Z5
        Z5Guessing(dR);

        std::cout << "  Z5 = " << Z5 << std::endl;

        FourRoundsAttack Z4_Guessing;
        uint64_t Z4 = Z4_Guessing.start(R4, dX3, operationsCounter5);

        std::cout << "  Z4 = " << Z4 << std::endl;

        Z4 = L(P(Z4));          // Z4 проходит P и L преобразования
        Z5 = S_Inverted(Z5);    // Z5 проходит обратное S преобразование

        std::array<uint64_t, 9> roundKeys;
        roundKeys[4] = Z4 ^ Z5;  // Ключ 5-го раунда находится через XOR Z4 и Z5 
        
                                 
       // Теперь можно вычислить все оставшиеся ключи и H
        for (int i = 3; i >= 0; i--) {
            roundKeys[i] = S_Inverted(P(L_Inverted(roundKeys[i + 1])));
            roundKeys[i] = roundKeys[i] ^ roundKeyConstant[i + 1];
        }

        std::cout << "Round keys:" << std::endl;
        for (int i = 4; i >= 0; i--) {
            std::cout << "  K[" << i + 1 << "] = " << roundKeys[i] << std::endl;
        }

        uint64_t H = S_Inverted(P(L_Inverted(roundKeys[0]))) ^ roundKeyConstant[0];
        std::cout << "  H = " << H << std::endl;
                                 
        std::cout << "Attack finished in " << (clock() - start_time) / (double)CLOCKS_PER_SEC / 60 << " minutes" << std::endl;
        std::cout.unsetf(std::ios::hex);
        std::cout.setf(std::ios::dec);
        std::cout << "Time complexity (total number of performed operations) is " << (operationsCounter5/pow(2,7)) << std::endl;

        return H;
    }
};