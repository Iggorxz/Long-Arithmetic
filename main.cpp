#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <vector>

using namespace std::chrono;
using namespace std;

uint64_t hexCharToValue(char c) {
    if (isdigit(c)) {
        return c - '0';
    }
    else if (isalpha(c)) {
        return toupper(c) - 'A' + 10;
    }
    else {
        throw invalid_argument("Invalid hex input");
    }
}

char hexValueToChar(uint64_t n) {
    if (n < 10) {
        return static_cast<char>('0' + n);
    }
    else if (n < 16) {
        return static_cast<char>('A' + n - 10);
    }
    else {
        throw invalid_argument("Invalid hex input");
    }
}

uint64_t* hexStringToNumbers(const string& s) {
    string input = s;

    while (input.length() % 8 != 0 || input.length() != 512) {
        input = '0' + input;
    }

    reverse(input.begin(), input.end());
    transform(input.begin(), input.end(), input.begin(), ::toupper);

    uint64_t* temp = new uint64_t[input.length()];

    for (int i = 0; i < input.length(); i++) {
        temp[i] = hexCharToValue(input[i]);
    }

    return temp;
}

uint64_t* hexStringTo32(const string& s) {
    uint64_t* temp = hexStringToNumbers(s);
    uint64_t* result = new uint64_t[65];
    for (int i = 63; i >= 0; i--) {
        result[i] = 0;
        for (int j = 0; j < 8; j++) {
            result[i] |= (temp[(63 - i) * 8 + j] << (4 * j));
        }
    }
    reverse(result, result + 64);
    return result;
}

string numbersToHexString(const uint64_t* n, const uint64_t length = 64) {
    string result;
    for (int i = length - 1; i >= 0; i--) {
        for (int j = 28; j >= 0; j -= 4) {
            result += hexValueToChar((n[i] >> j) & 0xF);
        }
    }
    result.erase(0, min(result.find_first_not_of('0'), result.size() - 1));
    return result;
}

uint64_t* numbersToBinary(uint64_t* n, const uint64_t length = 64) {
    uint64_t* result = new uint64_t[length * 32]();
    for (int i = length - 1; i >= 0; i--) {
        for (int j = 31; j >= 0; j--) {
            result[32 * i + j] = (n[i] >> j) & 1;
        }
    }
    return result;
}

uint64_t* binaryToNumbers(uint64_t* n, const uint64_t length = 64) {
    uint64_t* result = new uint64_t[length];
    int i = length - 1;
    do {
        result[i] = 0;
        int j = 31;
        do {
            result[i] |= (n[i * 32 + j] << j);
        } while (j--);
    } while (i--);
    return result;
}

uint64_t* longAdd(uint64_t* a, uint64_t* b, const uint64_t length = 64) {
    uint64_t* c = new uint64_t[length];
    uint64_t temp;
    uint64_t mod = 4294967295;
    c[length] = 0;

    int i = 0;
    do {
        temp = a[i] + b[i] + c[length];
        c[i] = temp & mod;
        c[length] = temp >> 32;
    } while (++i < length);

    // std::cout << numbersToHexString(c, 129) << std::endl;

    return c;
}

int longCmp(const uint64_t* a, const uint64_t* b, const uint64_t length = 64) {
    for (int i = length - 1; i >= 0; --i) {
        if (a[i] > b[i])
            return 1;
        else if (a[i] < b[i])
            return -1;
    }
    return 0;
}

uint64_t* longSub(const uint64_t* a, const uint64_t* b, const uint64_t length = 64) {
    uint64_t* c = new uint64_t[length];
    uint64_t borrow = 0;

    for (int i = 0; i < length; i++) {
        int64_t temp = static_cast<int64_t >(a[i]) - static_cast<int64_t>(b[i]) - static_cast<int64_t>(borrow);

        if (temp >= 0) {
            c[i] = static_cast<uint64_t>(temp);
            borrow = 0;
        }
        else {
            c[i] = static_cast<uint64_t>(temp + (1ULL << 32));
            borrow = 1;
        }
    }

    // c[length] = 0;

    if (borrow == 1) {
        cout << "Negative number" << endl;
        for (int i = 0; i < length; i++) {
            c[i] = 0;
        }
    }

    return c;
}

uint64_t* longMulOneDigit(const uint64_t* a, const uint64_t b, const uint64_t length = 64) {
    uint64_t* c = new uint64_t[length];
    uint64_t temp;
    uint64_t bits = 32;
    uint64_t mod = 4294967295;
    c[length] = 0;

    int i = 0;
    do {
        temp = a[i] * b + c[length];
        c[i] = temp & mod;
        c[length] = temp >> bits;
    } while (++i < length);

    return c;
}

uint64_t* longShiftDigitsToHigh(const uint64_t* a, uint64_t b, const uint64_t length = 64) {

    std::vector<uint64_t> temp(a, a + length);
    std::rotate(temp.rbegin(), temp.rbegin() + b, temp.rend());

    uint64_t* result = new uint64_t[length];
    std::copy(temp.begin(), temp.end(), result);

    return result;
}



uint64_t* longMul(const uint64_t* a, const uint64_t* b, const uint64_t length = 64) {
    uint64_t* c = new uint64_t[length]();
    uint64_t* temp;
    // memset(c, 0, 257 * sizeof(uint64_t));
    for (int i = 0; i < length; i++) {
        uint64_t* temp = longMulOneDigit(a, b[i], length);
        temp = longShiftDigitsToHigh(temp, i, length);
        uint64_t* tempSum = longAdd(c, temp, length);

        c = tempSum;
    }
    return c;
}

uint64_t* longSquareTemp(uint64_t* a, const uint64_t length = 64) {
    return longMul(a, a, length);
}

uint64_t bitLength(uint64_t* a, const uint64_t length = 2048) {
    uint64_t n = 0;
    int i = length - 1;

    while (i >= 0 && a[i] != 1) {
        i--;
    }

    while (i >= 0) {
        n++;
        i--;
    }

    return n;
}

uint64_t* longShiftBitsToHigh(uint64_t* a, uint64_t b, const uint64_t length = 2048) {
    uint64_t* temp = new uint64_t[length];
    memcpy(temp, a, length * sizeof(uint64_t));

    //   std::cout << "a_1 " << numbersToHexString(binaryToNumbers(a, length), length) << std::endl;

    while (b != 0) {
        rotate(temp, temp + length - 1, temp + length);
        temp[0] = 0;
        b--;
    }
    // std::cout << "temp " << numbersToHexString(temp, length) << std::endl;
    return temp;
}

int longCmpBit(const uint64_t* a, const uint64_t* b, const uint64_t length = 2048) {
    for (int i = length - 1; i >= 0; --i) {
        if (a[i] > b[i]) return 1;
        else if (a[i] < b[i]) return -1;
    }
    return 0;
}

pair<uint64_t*, uint64_t*> longDivMod(uint64_t* a, uint64_t* b, const uint64_t length = 64) {
    uint64_t* q = new uint64_t[length * 32]();
    uint64_t* r = numbersToBinary(a, length);
    uint64_t* c;
    uint64_t t;
    uint64_t* b_bin = numbersToBinary(b, length);
    uint64_t k = bitLength(b_bin, length * 32);

    //  std::cout << "r: " << numbersToHexString(binaryToNumbers(r, length), length) << std::endl;
    //  std::cout << "b_bin: " << numbersToHexString(binaryToNumbers(b_bin, length), length) << std::endl;

    uint64_t* longPower(uint64_t* a, uint64_t* b, const string& hex_b) {
    uint64_t* hex_num_b = hexStringToNumbers(hex_b);
    uint64_t* result = new uint64_t[4096]();
    result[0] = 1;

    uint64_t** powers = new uint64_t*[15]();
    powers[0] = hexStringTo32("1");
    powers[1] = a;

    for (int i = 2; i < 15; i++) {
        powers[i] = longMul(powers[i - 1], a);
    }

    for (int i = hex_b.length() - 1; i >= 0; i--) {
        result = longMul(result, powers[hex_num_b[i]]);

        if (i != 0) {
            for (int k = 1; k <= 4; k++) {
                result = longMul(result, result);
            }
        }
    }

    return result;
}

    while (longCmpBit(r, b_bin, length * 32) >= 0) {
        t = bitLength(r, length * 32);

        //  std::cout << "size: " << t << std::endl;

        c = longShiftBitsToHigh(b_bin, t - k, length * 32);

        //  std::cout << "c_1 " << numbersToHexString(binaryToNumbers(c, length), length) << std::endl;

        if (longCmpBit(r, c, length * 32) == -1) {
            t--;
            delete[] c;
            c = longShiftBitsToHigh(b_bin, t - k, length * 32);
            //    std::cout << "c_2 " << numbersToHexString(binaryToNumbers(c, length), length) << std::endl;
        }
        r = numbersToBinary(longSub(binaryToNumbers(r, length), binaryToNumbers(c, length), length), length);
        q[t - k] = 1;
        delete[] c;

        // std::cout << "r: " << numbersToHexString(binaryToNumbers(r, length), length) << std::endl;
    }
    return make_pair(binaryToNumbers(q, length), binaryToNumbers(r, length));
}

uint64_t* longShiftBitsToLow(const uint64_t* a, uint64_t b) {
    uint64_t* temp = new uint64_t[2048];
    copy(a, a + 2048, temp);
    rotate(temp, temp + b, temp + 2048);
    fill_n(temp + 2048 - b, b, 0);
    return temp;
}

uint64_t* longShiftDigitsToLow(const uint64_t* a, uint64_t b, const uint64_t length = 64) {
    uint64_t* temp = new uint64_t[length];
    copy(a, a + length, temp);
    while (b != 0) {
        rotate(temp, temp + 1, temp + length);
        temp[length - 1] = 0;
        b--;
    }
    return temp;
}

uint64_t* gcd(uint64_t* a, uint64_t* b, const uint64_t length = 64) {
    uint64_t* temp = new uint64_t[2048];
    uint64_t* res = new uint64_t[65];
    uint64_t* a_bit = numbersToBinary(a);
    uint64_t* b_bit = numbersToBinary(b);
    uint64_t* zero = new uint64_t[2048];

    fill_n(zero, 2048, 0);

    uint64_t* d = new uint64_t[2048]();
    d[0] = 1;

    if (a_bit[0] == 0 && b_bit[0] == 0) {
        int shiftCount = 0;

        while (a_bit[shiftCount] == 0 && b_bit[shiftCount] == 0) {
            shiftCount++;
        }

        a_bit = longShiftBitsToLow(a_bit, shiftCount);
        b_bit = longShiftBitsToLow(b_bit, shiftCount);
        d = longShiftBitsToHigh(d, shiftCount);
    }
    if (a_bit[0] == 0) {
        int shiftCount = 0;

        while (a_bit[shiftCount] == 0) {
            shiftCount++;
        }

        a_bit = longShiftBitsToLow(a_bit, shiftCount);
    }
    while (longCmpBit(b_bit, zero) != 0) {
        int shiftCount = 0;

        while (b_bit[shiftCount] == 0) {
            shiftCount++;
        }

        b_bit = longShiftBitsToLow(b_bit, shiftCount);

        copy(a_bit, a_bit + 2048, temp);

        int cmpResult = longCmpBit(temp, b_bit);
        a_bit = (cmpResult == 1) ? b_bit : a_bit;
        b_bit = numbersToBinary(longSub(binaryToNumbers((cmpResult == 1) ? temp : b_bit), binaryToNumbers((cmpResult == 1) ? b_bit : temp)));
    }
    res = longMul(binaryToNumbers(d), binaryToNumbers(a_bit));
    return res;
}

uint64_t* lcm(uint64_t* a, uint64_t* b) {
    uint64_t* newA = new uint64_t[129]();
    uint64_t* newB = new uint64_t[129]();
    uint64_t* newGcdResult = new uint64_t[129]();
    uint64_t* newResult = new uint64_t[64]();

    for (int64_t i = 0; i < 64; ++i)
    {
        newA[i] = a[i];
        newB[i] = b[i];
    }


    uint64_t* product = longMul(newA, newB, 129);
    uint64_t* gcdResult = gcd(a, b);

    for (int64_t i = 0; i < 64; ++i)
    {
        newGcdResult[i] = gcdResult[i];
    }

    uint64_t* result = longDivMod(product, newGcdResult, 129).first;

    for(int64_t i = 0; i < 64; ++i)
    {
        newResult[i] = result[i];
    }

    return newResult;
}

uint64_t* barrettReduction(uint64_t* x, uint64_t* n) {
    if (longCmp(x, longSquareTemp(n)) == 1) {
        x = longDivMod(x, n).second;
        return x;
    }
    n[64] = 0;
    uint64_t* b = new uint64_t[65]();

    b[0] = 1;
    uint64_t k = 64;
    int i = 63;
    while (i >= 0 && n[i] == 0) {
        k--;
        i--;
    }

    uint64_t* mu = longDivMod(longShiftDigitsToHigh(b, 2 * k), n).first;
    uint64_t* q = longShiftDigitsToLow(x, k - 1);
    q = longMul(q, mu);
    q = longShiftDigitsToLow(q, k + 1);
    uint64_t* r = longSub(x, longMul(q, n));
    // std::cout << numbersToHexString(n) << std::endl;
    while (longCmp(r, n) == 1) {
        r = longSub(r, n);
    }
    return r;
}

uint64_t* barrettReduction(uint64_t* x, uint64_t* n, uint64_t* mu, const uint64_t length = 64) {
    if (longCmp(x, longSquareTemp(n, length), length) == 1) {
        x = longDivMod(x, n, length).second;
        return x;
    }
    // n[64] = 0;
    uint64_t* b = new uint64_t[length]();

    b[0] = 1;
    uint64_t k = length;
    int i = length - 1;
    while (i >= 0 && n[i] == 0) {
        k--;
        i--;
    }

    //    std::cout << "k: " << k << std::endl;
    //    std::cout << "x: " << numbersToHexString(x) << std::endl;
    //
    uint64_t* q = longShiftDigitsToLow(x, k - 1, length);
    //    std::cout << "q1: " << numbersToHexString(q) << std::endl;
    q = longMul(q, mu, length);
    //    std::cout << "mu: " << numbersToHexString(mu) << std::endl;
    //    std::cout << "q2: " << numbersToHexString(q) << std::endl;
    q = longShiftDigitsToLow(q, k + 1, length);
    //    std::cout << "q3: " << numbersToHexString(q) << std::endl;
    auto mult = longMul(q, n, length);
    //    std::cout << numbersToHexString(mult) << std::endl;
    uint64_t* r = longSub(x, mult, length);
    //    std::cout << "first r: " << numbersToHexString(r) << std::endl;

    while (longCmp(r, n, length) >= 1) {
        //    std::cout << "r: ";
        //    std::cout << numbersToHexString(r) << std::endl;
        //    std::cout << "n: ";
        //    std::cout << numbersToHexString(n) << std::endl;

        r = longSub(r, n, length);
    }

    delete[] b;

    return r;
}

uint64_t* longAddBarrett(uint64_t* a, uint64_t* b, uint64_t* n) {
    uint64_t k = 64;

    int i = 63;
    while (i >= 0 && n[i] == 0) {
        k--;
        i--;
    }

    uint64_t* temp = new uint64_t[129]();
    temp[0] = 1;

    uint64_t* newN = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newN[i] = n[i];
    }

    uint64_t* mu = longDivMod(longShiftDigitsToHigh(temp, 2 * k, 129), newN, 129).first;
    auto additionResult = longAdd(a, b, 129);
    return barrettReduction(additionResult, n, mu);
}

uint64_t* longSubBarrett(uint64_t* a, uint64_t* b, uint64_t* n) {
    uint64_t k = 64;

    int i = 63;
    while (i >= 0 && n[i] == 0) {
        k--;
        i--;
    }

    uint64_t* temp = new uint64_t[129]();
    temp[0] = 1;

    uint64_t* newN = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newN[i] = n[i];
    }

    uint64_t* mu = longDivMod(longShiftDigitsToHigh(temp, 2 * k, 129), newN, 129).first;
    auto additionResult = longAdd(a, b, 129);

    int cmpResult = longCmp(a, b);

    if (cmpResult == 0) {
        return new uint64_t[1]();
    }
    else if (cmpResult == -1) {
        uint64_t* result = longSub(b, a);
        result = barrettReduction(result, n, mu);
        return longSub(n, result);
    }
    else {
        uint64_t* result = longSub(a, b);
        result = barrettReduction(result, n, mu);
        return result;
    }
}

// число яке повертається розміром 129
uint64_t* evaluateMu(const uint64_t* module)
{
    uint64_t k = 64;

    int i = 63;
    while (i >= 0 && module[i] == 0) {
        k--;
        i--;
    }

    uint64_t* temp = new uint64_t[129]();
    temp[0] = 1;

    uint64_t* newN = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newN[i] = module[i];
    }

    return longDivMod(longShiftDigitsToHigh(temp, 2 * k, 129), newN, 129).first;
}

uint64_t* longMulBarrett(uint64_t* a, uint64_t* b, uint64_t* n) {
    uint64_t* newA = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newA[i] = a[i];
    }

    uint64_t* newB = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newB[i] = b[i];
    }

    uint64_t* newN = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newN[i] = n[i];
    }

    auto temp = longMul(newA, newB, 129);
    auto mu = evaluateMu(n);
    auto multResult = barrettReduction(temp, newN, mu, 129);

    uint64_t* multResultCopy = new uint64_t[64]();
    for(int i = 0; i < 64; ++i)
    {
        multResultCopy[i] = multResult[i];
    }

    return multResultCopy;
}

uint64_t* longSqrBarrett(uint64_t* a, uint64_t* n) {
    return longMulBarrett(a, a, n);
}

uint64_t* longModPowerBarrett(uint64_t* a, uint64_t* b, uint64_t* n) {
    uint64_t* b_bit = numbersToBinary(b, 129);
    uint64_t b_len = bitLength(b_bit);
    uint64_t* c = new uint64_t[129]();
    uint64_t k = 64;

    c[0] = 1;

    int i = 63;
    while (i >= 0 && n[i] == 0) {
        k--;
        i--;
    }

    // std::cout << k << std::endl;
    uint64_t* temp = new uint64_t[129]();
    temp[0] = 1;

    uint64_t* newN = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newN[i] = n[i];
    }
    // std::cout << numbersToHexString(newN, 129) << std::endl;


    auto mu = evaluateMu(n);
    std::cout << "mu: " << numbersToHexString(mu, 129) << std::endl;

    uint64_t* newA = new uint64_t[129]();
    for (int64_t i = 0; i < 64; ++i)
    {
        newA[i] = a[i];
    }

    newA = longDivMod(newA, newN, 129).second;
    // std::cout << "newA before cycle: " << numbersToHexString(newA, 129) << std::endl;

    for (i = 0; i < b_len; i++) {
        // std::cout << "iteration: " << i << std::endl;
        if (b_bit[i] == 1) {
            c = barrettReduction(longMul(c, newA, 129), newN, mu, 129);
            // std::cout << "c: " << numbersToHexString(c, 129) << std::endl;
        }
        newA = barrettReduction(longSquareTemp(newA, 129), newN, mu, 129);
        // std::cout << "newA: " << numbersToHexString(newA, 129) << std::endl;
    }

    delete[] temp;

    uint64_t* result = new uint64_t[64]();
    for (std::size_t i = 0; i < 64; ++i)
    {
        result[i] = c[i];
    }

    delete[] c;
    delete[] newA;
    delete[] newN;

    return result;
}

template<typename Func>
void measureAndPrint(const std::string& operation, Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << operation << ": " << result << std::endl
              << "length: " << result.length() << std::endl
              << "time: " << duration.count() << " microseconds" << std::endl << std::endl;
}

int main() {


    string a_0 = "23C0D0050AE991EF232C32AA88639EF38290F68434288F9C4BF429FE499B9220ABCA87E41472A1E15199B20E6CFDFE13CBB4E1F9358760F04A0FD9EA8A2B5A98E2CF08A5D0BFCC4C75B0F0A7F7BC57EFBDF2ACDC6CA08F63DA3B3A040CFA1AABC5F07A4B843282AD6E6FD1ECB9A00800F46446DCD357ACA4BE192CBF38282A9A";
    string b_0 = "49E8FBC709682FD27B5374521000A9F7A84C1E31156EAF661DB2CEF3E738E9A05ED540487A805DD5098D19B5DD1EED610CFF655279E2BE39FB520C7713EB41258886210005A46E6DE9311231B85DA6D4F32C028847AA64BC04458861BE442512DB2056BAE4A1D44D10D7013DDB5F8DCAB1CC17F535D080974A219D4B0177FBF9";
    string n_0 = "5BFFD2AD88C901A224CABA35BA09C26AE6BE9C06DAE976A0FC91B7E6F5076559813CB94D2B33E86C9BF1AFABC4598E769FD3E7C27BC45BBEAB5E78CE48FABE89EF514FF65C64DA6E6858A91600DE5B4091B219B88FF502FD166EA1EF805E37BC0695D17348823898C0A7BFE217D7980CA93B724B2AECEC6C4F0165E9B30002E2";

    cout << "input a: " << a_0 << endl << "length: " << a_0.length() << endl << endl;
    cout << "input b: " << b_0 << endl << "length: " << b_0.length() << endl << endl;
    cout << "input n: " << n_0 << endl << "length: " << n_0.length() << endl << endl;


    measureAndPrint("gcd(a, b)", [&]() { return numbersToHexString(gcd(hexStringTo32(a_0), hexStringTo32(b_0))); });
    measureAndPrint("lcm(a, b)", [&]() { return numbersToHexString(lcm(hexStringTo32(a_0), hexStringTo32(b_0))); });

    measureAndPrint("(a+b) mod n", [&]() { return numbersToHexString(longAddBarrett(hexStringTo32(a_0), hexStringTo32(b_0), hexStringTo32(n_0))); });
    measureAndPrint("(a-b) mod n", [&]() { return numbersToHexString(longSubBarrett(hexStringTo32(a_0), hexStringTo32(b_0), hexStringTo32(n_0))); });

    measureAndPrint("(a*b) mod n", [&]() { return numbersToHexString(longMulBarrett(hexStringTo32(a_0), hexStringTo32(b_0), hexStringTo32(n_0))); });
    measureAndPrint("(a^2) mod n", [&]() { return numbersToHexString(longSqrBarrett(hexStringTo32(a_0), hexStringTo32(n_0))); });
    measureAndPrint("(a^b) mod n", [&]() { return numbersToHexString(longModPowerBarrett(hexStringTo32(a_0), hexStringTo32(b_0), hexStringTo32(n_0))); });

    return 0;
}