// ����� ������ 5 �������

class FiveRoundsAttack{

private:

    bool dX4_isFound = false;
    std::array<uint64_t, 15> dX4{};
    std::array<uint64_t, 15> dX3{};
    uint64_t Z5 = 0x0000000000000000LL;
    uint64_t operationsCounter5 = 0; // ���������� ��� �������� ���������� �������� � ����� �� 5 ������� (��������� ���������)


    // ������������ 1-�� ������� ����������. ������������ ��� ���������� 16 ����� � Y3.
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

    /* ������������ 60 - ������� �������� �� ������ 4 �����(���������) ������ �� 15 "���������" � d-"��������" dX4
     * ��� �������� � ������� backward */
    uint64_t firstHalfBytesFormation(const std::array<uint64_t, 15>& dDiff) {
        uint64_t firstHalfBytes = 0;
        for (int i = 0; i < 15; i++) {
            firstHalfBytes <<= 4;
            firstHalfBytes += dDiff[i] >> 60;
        }
        return firstHalfBytes;
    }

    // ����� ���������� �����������, �� ��������� 60-������ �������� �� 4 ����� num ���������, ��� num = [2,3,4]
    uint64_t halfBytesFormation(const std::array<uint64_t, 15>& dDiff, uint8_t num) {
        uint64_t HalfBytes = 0;
        for (int i = 0; i < 15; i++) {
            HalfBytes <<= 4;
            HalfBytes += ((dDiff[i] << (4*(num - 1))) >> 60);
        }
        return HalfBytes;
    }

    // ����� ������������ ��� ���������� ������� backward. ���������� ���������� �� ����������� ��� ���� ��������� backward[0]
    void vectorSort(std::vector<std::array<uint64_t, 2>>& backward) {
        std::cout << "Vector sorting has started " << std::endl;
        unsigned int sort_start_time = clock();
        std::make_heap(backward.begin(), backward.end());
        std::sort_heap(backward.begin(), backward.end());
        operationsCounter5 += 20*pow(2,20); // ��������� ����������
        std::cout << "Vector was sorted in " << ((clock() - sort_start_time) / (double)CLOCKS_PER_SEC) << " seconds" << std::endl;
    }


    /* �������� �����. ����� ���� ���������� ������ ���������� "���������", ���������� � ������ ���������������,
     * ����� ��������� ������� backward. ��������� ��������� ������: O(log n), ��� n - ���-�� ��������� �������.
     * ���-�� ��������� � backward �� ������ ������ ������ = 2^20   ==>   � ����� ������ ���������� �� ����� 20 �������� */
    bool binarySearch(const std::vector<std::array<uint64_t, 2>>& backward, uint64_t dX4_forward) {
        int left = 0;
        int right = backward.size() - 1;
        int mid;
        while (left <= right) {
            operationsCounter5++; // ��������� ����� �������� ��������� (20 �������� �� ���� ������� ������� forward)
            mid = (left + right) / 2;
            if (backward[mid][0] == dX4_forward) {
                Z5 += backward[mid][1]; // ���� ������� ����������, ������������ 16 ��������� ��������� ����� Z5
                Z5 <<= 48;
                return true;
            }
            else if (backward[mid][0] > dX4_forward) right = mid - 1;
            else left = mid + 1;
        }
        return false;
    }

    /* �����, �������������� � ������ ��������������� ��� ��������� ������������
     * d-"���������" dX4 � ���������� ������� �������� �������������� */
    bool forwardEqualityCheckUp(const std::vector<std::array<uint64_t, 15>>& forward,
                                const std::vector<std::array<uint64_t, 2>>& backward, uint8_t i, uint8_t j) {

        if (j > 0xf) return false;
        uint64_t x;
        std::array<uint64_t, 15> dCurrent;
        // ����� ������������ �� ��������� ����� - 2 ����� �� 16 �������� � ������
        for (uint8_t w = 0; w <= 0xf; w++) {                          // 16 ��������
            for (uint8_t z = 0; z <= 0xf; z++) {                      // 16 ��������
                if (dX4_isFound) return false;
                x = firstColumnFormation(i, j, w, z);
                for (uint32_t r = 0; r < forward.size(); r++) {       // 65 536 ��������
                    dCurrent = dL(dP(dS(forward.at(r), x)));
                    operationsCounter5 += 15; // ��������� ������ (�� 2^32) �������� ������� forward (������ ��������������)
                    /* ��������������
                     * S: dY3 -> dZ3
                     * P: dZ3 -> dW3
                     * L: dW3 -> dX4
                     *
                     * ���������� ���������� �� ������� ��������� � ���������� dX4 �� backward,
                     * �� ������ �� � ������ forward
                     *
                     * � �������� ������ 20 ��������  */
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
     * -------------------------- ������ �������������� --------------------------
     * 
     */

    bool forwardPropagations(std::array<uint64_t, 15> dM, uint64_t M0,
           const std::vector<std::array<uint64_t, 2>>& backward) {

        std::cout << "Forward propagations have started, first round key = 0000000000000000" << std::endl;
        unsigned int FP_start_time = clock();

        /* ������ � d-"����������" ������ ��������������. � �� ����� ��������� 2^16 �������� Y2.
         * ����������� �������� ����� ������������ ��������, �.�. � �� �������� ��� ������ */
        std::vector<std::array<uint64_t, 15>> forward;

        std::array<uint64_t, 15> dCurrent; // ������� d-"��������"

        uint64_t firstRoundKey = 0x0000000000000000LL;

        do {      // 16 ��������

            /* x ��� ������� S-�������������� (Y1) ��������� ����� XOR (X-��������������) �������� ������
             * � ����� ������� ������, �������� �������� �� ��������� */
            uint64_t x = M0 ^ firstRoundKey;

            // �������������� d-"���������" 1-�� ������
            dCurrent = dL(dP(dS(dM, x)));
            /* ��������������:
             * S: dY1 -> dZ1
             * P: dZ1 -> dW1
             * L: dW1 -> dX2
             *
             *
             * �������������� d-"���������" 2-�� ������
             * 
             * S-�������������� dY2 -> dZ2 */
            std::cout << "    Guessing 16 bits (row) of Y2: ";
            for (uint32_t i = 0; i <= 0xffff; i++) {
                x = i;
                x <<= 48;
                forward.insert(forward.end(), dS(dCurrent, x));
            }
            std::cout << "Done." << std::endl;
            /* ����� ����� S-�������������� � ������� ����� ��������� 65536 d-"���������"
             *
             * ��� ���������� �������������� ����� ������������� ��� ������ d-"��������" �� ������� ��������  */
            for (uint32_t i = 0; i < forward.size(); i++) {
                forward.at(i) = dP(forward.at(i));  // P: dZ2 -> dW2
                forward.at(i) = dL(forward.at(i));  // L: dW2 -> dX3
            }


            /* �������������� d-"���������" 3-�� ������
             * S-�������������� dY3 -> dZ3   */
            std::cout << "    Guessing 16 bits (column) of Y3: ";
            
            unsigned int FPw_start_time = clock();
            
            /* ������ ������������ 2^16 * 2^16 * 20 �������� ��������� dX4 � ������ � �������� ���������������.
             * ����� ��� �� ������� ��������� "���������" ��������� ������� backward. ��� ������ 
             * ������������ 6 ������������ ������� ��� ������ 12 �������� ����� � ���������� j � 4 ������ ��� ��������� 4-�  */
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

            // ���� �� ������� ����������, �� ���� ��������, � ���� ���� ���������� ������ 
            forward.clear();
            firstRoundKey += 0x1000000000000000LL; 
            if (firstRoundKey != 0) std::cout << "    Done, but no match was found. New first round key = " << firstRoundKey << std::endl;
        } while (firstRoundKey != 0);
          
        std::cout << "    dX4 wasn't found" << std::endl;
        std::cout << "Forward propagations finished in " << ((clock() - FP_start_time) / (double)CLOCKS_PER_SEC) / 60 << " minutes" << std::endl;
        return false;

    }

    /*
     *  -------------------------- �������� �������������� --------------------------
     *
     */
    std::vector<std::array<uint64_t, 2>> backwardPropagations(const std::array<uint64_t, 15>& dR, int columnNum) {

        if (columnNum == 1) std::cout << "Backward propagations have started " << std::endl;
        unsigned int BP_start_time = clock();

        // ����� �������������� ��� ���������� �������� 65 536 ��������� dY5
        std::vector<std::array<uint64_t, 15>> backwardBuff;

        /* ��� �������� ������ 15-������ d-"���������" � backward ����� ���������� �� ��� ��������:
         * 1) ������� ������ ���������� ���� 15 "������" ������ d-"��������" (60 ���) - backward[0]
         * 2) ��������� ���� Z5, ��������������� d-"��������" (16 ���)                - backward[1] */
        std::vector<std::array<uint64_t, 2>> backward;

        std::array<uint64_t, 15> dCurrent;
        std::array<uint64_t, 2> dCurr;

        uint64_t y;

        dCurrent = dP(dL_Inverted(dR)); // �������� L � P �������������� dR -> dZ5


        // S-�������������� dZ5 -> dY5
        if (columnNum == 1) std::cout << "    Guessing 16 bits (row) of Z5: ";
        for (uint32_t i = 0; i <= 0xffff; i++) {
            y = i;
            y <<= (64 - 16 * columnNum);
            backwardBuff.insert(backwardBuff.end(), dS_Inverted(dCurrent, y));
        }
        if (columnNum == 1) std::cout << "Done." << std::endl;

        /* ����� ����� S-�������������� � ������� ����� ��������� 65536 d-"���������"
         * ��� ���������� �������������� ����� ������������� ��� ������ d-"��������" �� ������� ��������  */

        // �������� L � P �������������� dX5 -> dZ4
        for (uint32_t i = 0; i < backwardBuff.size(); i++) {
            backwardBuff.at(i) = dL_Inverted(backwardBuff.at(i));
            backwardBuff.at(i) = dP(backwardBuff.at(i));
        }

        // S-�������������� dZ4 -> dY4
        if (columnNum == 1) std::cout << "    Guessing 4 bits of Z4: ";
        for (uint8_t i = 0; i <= 0xf; i++) {
            y = i;
            y <<= (64 - 4 * columnNum);

            /* ��� ������ ����������� ������������ 1 048 576 ��������� dX4, � ������� ��������
             * ������ ��������. � ���� ����� ������������ dX4 � ������ ���������������. */
            if (columnNum == 1) {
                for (uint32_t j = 0; j < backwardBuff.size(); j++) {
                    dCurr[0] = firstHalfBytesFormation(dS_Inverted(backwardBuff.at(j), y));
                    dCurr[1] = j;
                    backward.insert(backward.end(), dCurr);
                    operationsCounter5 += 15; // ��������� ������ (�� 2^20) �������� ������� backward (�������� ��������������)
                }
            }
            /* ��� ����������� 3-� ������������ ����� ����������� ������� ��������� 2, 3 � 4 ���������� 
             * � ���������������� ����������� X4, ������� � ����� ������� ��� ����� ��������. ���������
             * ��������� ���� ����� ��������� � Z5. */
            else {
                uint64_t dX4c = halfBytesFormation(dX4, columnNum);
                for (uint32_t j = 0; j < backwardBuff.size(); j++) {
                    operationsCounter5 += 15; // ��������� ������ (�� 3*2^20) �������� ������������ ��������� Z5
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

    // ����������� Z5
    void Z5Guessing(std::array<uint64_t, 15> dR) {

        std::cout << "Guessing of Z5 has started " << std::endl;
        unsigned int Z5G_start_time = clock();
        /* ������ ������� ���� ������� ��� ������ ����������� �������� ��������������
         * ��� ���������� ������� ����������� ����������� */
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

        // ����� 15 d-"������" dM
        std::array<uint64_t, 15> dM;
        for (int i = 1; i < 16; i++) {
            dM[i - 1] = M[0] ^ M[i];
        }
        // ����� 15 d-"������" dR
        std::array<uint64_t, 15> dR;
        for (int i = 1; i < 16; i++) {
            dR[i - 1] = R5[0] ^ R5[i];
        }

        // �������� ��������������. ������� 1 048 576 ��������� dX4 � ��������� ������ ����������
        std::vector<std::array<uint64_t, 2>> backward = backwardPropagations(dR, 1);

        // ���������� ��������� ������� ��� ����������� ������������� ������ ��������� ������
        vectorSort(backward);

        /* ������ ��������������. ���� ����� 4 294 967 296 ��������� dX4 ���������� �� ������� ���������
         * � ����� �� 1 048 576 ��������� dX4 �� ������� backward (� ������ ����������� ������ ���� ��������� dX4) */
        forwardPropagations(dM, M[0], backward);

        for (int h = 0; h < 15; h++) {
            std::cout << "  dX4[" << h << "] = " << dX4[h] << std::endl;
        }

        // ���������� ���������� ������� Z5
        Z5Guessing(dR);

        std::cout << "  Z5 = " << Z5 << std::endl;

        FourRoundsAttack Z4_Guessing;
        uint64_t Z4 = Z4_Guessing.start(R4, dX3, operationsCounter5);

        std::cout << "  Z4 = " << Z4 << std::endl;

        Z4 = L(P(Z4));          // Z4 �������� P � L ��������������
        Z5 = S_Inverted(Z5);    // Z5 �������� �������� S ��������������

        std::array<uint64_t, 9> roundKeys;
        roundKeys[4] = Z4 ^ Z5;  // ���� 5-�� ������ ��������� ����� XOR Z4 � Z5 
        
                                 
       // ������ ����� ��������� ��� ���������� ����� � H
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