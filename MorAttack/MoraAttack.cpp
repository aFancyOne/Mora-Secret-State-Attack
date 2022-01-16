
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <thread>
#include <ctime>
#include <algorithm>

#include "StateTransformations.h"
#include "dDifferenceTransformations.h"

#include "4RoundsAttack.h"
#include "5RoundsAttack.h"
#include "MoraCF.h"

int main()  
{
    // Смена флага cout для вывода на экран 16-ричных значений по-умолчанию
    std::cout.unsetf(std::ios::dec);
    std::cout.setf(std::ios::hex);

    /* Выбор начальных параметров:
     * d + 1 = 16 начальных входов
     * d + 1 = 16 выходов 5-го и 4-го раунда соответственно */
    std::array<uint64_t, 16> M  = { 0xf000000000000000LL, 0x0000000000000000LL, 0x1000000000000000LL, 0x2000000000000000LL,
                                    0x3000000000000000LL, 0x4000000000000000LL, 0x5000000000000000LL, 0x6000000000000000LL, 
                                    0x7000000000000000LL, 0x8000000000000000LL, 0x9000000000000000LL, 0xa000000000000000LL, 
                                    0xb000000000000000LL, 0xc000000000000000LL, 0xd000000000000000LL, 0xe000000000000000LL };

    std::array<uint64_t, 16> R5 = { 0xaf8a430741db8d29LL, 0x24b348da07cc0364LL, 0xfa983a1fcdb43131LL, 0x1f6849c38ec20901LL, 
                                    0xbfadb96074e0f060LL, 0x7545c21d73d0e15cLL, 0x15a90df216cc087cLL, 0x545b3de64afb4ba1LL, 
                                    0xfb1bf0e7d0093960LL, 0x2cdd6cbc4debe43cLL, 0x93ce510fe71aac6dLL, 0x9933e974083af783LL,
                                    0x6a3ed4efdec17d83LL, 0x6b448ac7a75715b1LL, 0x98ee462c979af4c3LL, 0x0f508608070c0019LL };

    std::array<uint64_t, 16> R4 = { 0xe4ceb3f5cf8f4480LL, 0x5d255e9b641d3ebeLL, 0xad8428881c5dbea6LL, 0xfeb1ccb0c5a988b0LL,
                                    0x8d6e8d1e6e64babfLL, 0xeeeabda18c8ec190LL, 0xe77fc19e3ce85e16LL, 0xcba7feac18bc73e7LL,
                                    0xc3749b6af70de395LL, 0x4df1602f7b185e71LL, 0x73ddecbca8ffab05LL, 0xee971d82db5337c9LL,
                                    0xc8c4b27d54b46f68LL, 0x00a7e9d712092236LL, 0x11a50d5d96f982fdLL, 0xdea38bcfa52f821eLL };

    
    // (Опционально) Получение выходов 5-го и 4-го раундов для каждого входа M[i]
    /*for (int i = 0; i < M.size(); i++) {
        MoraCF test(M[i], 0x63d9b05de17ffb26LL);
        test.compressionFunction();
    }*/

    // Запуск атаки против 5 раундов
    FiveRoundsAttack attack;
    uint64_t H = attack.start(M, R5, R4);
    
    return 0;
}