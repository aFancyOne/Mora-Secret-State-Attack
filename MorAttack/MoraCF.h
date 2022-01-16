// Mora Compresion Function - реализация функции сжатия хэш-функции "Мора"

class MoraCF{

private:
	uint64_t M; // Входное сообщение 
	uint64_t H; // Входное состояние

	// Формирование цепочки раундовых ключей
	std::array<uint64_t, 9> createKeys(const uint64_t& h) {
		std::array<uint64_t, 9> roundKey;
		roundKey[0] = L(P(S(h^roundKeyConstant[0])));
		for (int i = 1; i < 9; i++) {
			roundKey[i] = roundKey[i - 1] ^ roundKeyConstant[i];
			roundKey[i] = S(roundKey[i]);
			roundKey[i] = P(roundKey[i]);
			roundKey[i] = L(roundKey[i]);
		}
		return roundKey;
	}

public:
	MoraCF(uint64_t input_M, uint64_t input_H) {
		M = input_M;
		H = input_H;
	}

	// Функция сжатия
	void compressionFunction() {
		uint64_t R; // Выход

		// Генерация ключей
		std::array<uint64_t, 9> keys = createKeys(H);

		std::array<uint64_t, 16> R5;
		std::array<uint64_t, 16> R4;

		// Основные преобразования
		R = L(P(S(M ^ keys[0])));
		for (int i = 1; i < 9; i++) {
			R = R ^ keys[i];
			R = S(R);
			R = P(R);
			R = L(R);
			if (i == 3 || i == 4) std::cout << "R after " << i + 1 << " rounds = " << R << std::endl;
			if (i == 4) std::cout << std::endl;
		}
	}
};


