// Атака против 4 раундов (часть атаки против 5 раундов, необходимая для нахождения Z4)

class FourRoundsAttack{

private:

    uint64_t Z4 = 0;
    uint64_t operationsCounter4 = 0; // Переменная для подсчёта количества операций в атаке на 4 раунда

    // Формирование 60-битного значения из 4 битов (полубайта) каждой из 15 "разностей" в d-"разности" dX3.
    uint64_t halfBytesFormation(const std::array<uint64_t, 15>& dDiff, int num) {
        uint64_t HalfBytes = 0;

        for (int i = 0; i < 15; i++) {
            HalfBytes <<= 4;
            HalfBytes += ((dDiff[i] << (4 * (num - 1))) >> 60);
        }
        return HalfBytes;
    }


    bool backwardPropagations(const std::array<uint64_t, 15>& dR, const std::array<uint64_t, 15>& dX3, int columnNum) {

        std::vector<std::array<uint64_t, 15>> backwardBuff;
        std::array<uint64_t, 15> dCurrent;
        uint64_t y;

        dCurrent = dP(dL_Inverted(dR)); // Обратные L и P преобразования dR -> dZ4

        // S-преобразование dZ4 -> dY4
        for (uint32_t i = 0; i <= 0xffff; i++) {
            y = i;
            y <<= (64 - 16 * columnNum);
            backwardBuff.insert(backwardBuff.end(), dS_Inverted(dCurrent, y));
        }

         // Обратные L и P преобразования dX4 -> dZ3
        for (uint32_t i = 0; i < backwardBuff.size(); i++) {
            backwardBuff.at(i) = dL_Inverted(backwardBuff.at(i));
            backwardBuff.at(i) = dP(backwardBuff.at(i));
        }

        // S-преобразование dZ3 -> dY3
        for (uint8_t i = 0; i <= 0xf; i++) {
            y = i;
            y <<= (64 - 4 * columnNum);

            uint64_t dX3c = halfBytesFormation(dX3, columnNum);
            for (uint32_t j = 0; j < backwardBuff.size(); j++) {
                operationsCounter4 += 15; // Сложность одного (из 4*2^20) варианта полубайта Z4
                if (dX3c == halfBytesFormation(dS_Inverted(backwardBuff.at(j), y), columnNum)) {
                    y = j;
                    y <<= (64 - 16 * columnNum);
                    Z4 += y;
                    std::cout << "    Guessing of " << columnNum << " row of Z4 was successful" << std::endl;
                    return true;
                }
            }
        }
        std::cout << "    Guessing of " << columnNum << " row of Z4 has failed" << std::endl;
        return false;
    }

    // Построчное угадывание Z4
    void Z4Guessing(const std::array<uint64_t, 15>& dR, const std::array<uint64_t, 15>& dX3) {

        std::cout << "Guessing of Z4 has started " << std::endl;
        unsigned int Z4G_start_time = clock();

        // На каждую строчку свой поток (всего 4 потока)
        std::thread t_firstRow(&FourRoundsAttack::backwardPropagations, this, std::ref(dR), std::ref(dX3), 1);
        std::thread t_secondRow(&FourRoundsAttack::backwardPropagations, this, std::ref(dR), std::ref(dX3), 2);
        std::thread t_thirdRow(&FourRoundsAttack::backwardPropagations, this, std::ref(dR), std::ref(dX3), 3);
        std::thread t_fourthRow(&FourRoundsAttack::backwardPropagations, this, std::ref(dR), std::ref(dX3), 4);
        t_firstRow.join();
        t_secondRow.join();
        t_thirdRow.join();
        t_fourthRow.join();
        std::cout << "Guessing of Z4 finished in " << (clock() - Z4G_start_time) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
    }

public:

    FourRoundsAttack() {}

    uint64_t start(const std::array<uint64_t, 16>& R, const std::array<uint64_t, 15>& dX3, uint64_t& operationsCounter5) {

        // Поиск 15 разниц dR
        std::array<uint64_t, 15> dR;
        for (int i = 1; i < 16; i++) {
            dR[i - 1] = R[0] ^ R[i];
        }

        Z4Guessing(dR, dX3);

        operationsCounter5 += operationsCounter4;
        return Z4;
    }
};
