// Заголовочный файл с методами прямых и обратных преобразований для d-"разностей"

// P - преобразование d-"разностей". Каждый элемент d-"разности" проходит P-преобразование отдельно
std::array<uint64_t, 15> dP(const std::array<uint64_t, 15> &d_input) {
    std::array<uint64_t, 15> d_output;
    for (int i = 0; i < 15; i++) {
        d_output[i] = P(d_input[i]);
    }
    return d_output;
}

// L-преобразование d-"разностей". Каждый элемент d-"разности" проходит L-преобразование отдельно
std::array<uint64_t, 15> dL(const std::array<uint64_t, 15> &d_input) {
    std::array<uint64_t, 15> d_output;
    for (int i = 0; i < 15; i++) {
        d_output[i] = L(d_input[i]);
    }
    return d_output;
}

// Обратное L - преобразование d-"разностей". Каждый элемент d-"разности" проходит обратное L-преобразование отдельно
std::array<uint64_t, 15> dL_Inverted(const std::array<uint64_t, 15> &d_input) {
    std::array<uint64_t, 15> d_output;
    for (int i = 0; i < 15; i++) {
        d_output[i] = L_Inverted(d_input[i]);
    }
    return d_output;
}

// S - преобразование d-"разностей". Для прохождения S-преобразования необходим x
std::array<uint64_t, 15> dS(const std::array<uint64_t, 15> &d_input, const uint64_t &x) {
    std::array<uint64_t, 15> d_output;
    for (int i = 0; i < 15; i++) {
        d_output[i] = S(x) ^ S(x ^ d_input[i]);
    }
    return d_output;
}

// Обратное S - преобразование d-"разностей".  Для прохождения обратного S-преобразования необходим y
std::array<uint64_t, 15> dS_Inverted(const std::array<uint64_t, 15> &d_input, const uint64_t &y) {
    std::array<uint64_t, 15> d_output;
    for (int i = 0; i < 15; i++) {
        d_output[i] = S_Inverted(y) ^ S_Inverted(y ^ d_input[i]);
    }
    return d_output;
}