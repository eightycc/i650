//
//  biq.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/23/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"

using namespace std;

ostream& operator<<(ostream& s, const BiqDigit& v) {
    v.print(s);
    return s;
}

string BiqWord::signedDecimal() const {
    string v;
    v = (digits[dd0].biq() == BiqDigit::biqMinus) ? "-"
      : (digits[dd0].biq() == BiqDigit::biqPlus)  ? "+"
      : (digits[dd0].biq() == BiqDigit::biqBlank) ? "_"
      : "X";
    for (int idx = BiqWord::dd10; idx >= BiqWord::dd1; idx--) {
        v += digits[idx].character();
    }
    return v;
}

void BiqWord::signedDecimal(const string& v) {
    string v1 = v;
    if (v1.empty()) {
        return;
    }
    char sign = '+';
    if (v1[0] == '+' || v1[0] == '-') {
        sign = v1[0];
        v1.erase(0, 1);
    }
    if (v1.empty()) {
        return;
    }
    while (v1[0] == '0') {
        v1.erase(0, 1);
    }
    if (v1.size() > 10) {
        return;
    }
    for (auto ch : v1) {
        if (ch < '0' || ch > '9')
            return;
    }
    for (int i = dd1; i <= dd10; i++) {
        digits[i].biq(BiqDigit::biq0);
    }
    int i = dd1;
    while (!v1.empty()) {
        digits[i++].decimal(v1.back() - '0');
        v1.pop_back();
    }
    digits[dd0] = (sign == '+') ? BiqDigit::biqPlus
                                : BiqDigit::biqMinus;
}
    
int64_t BiqWord::binary() const {
    if (!isValid()) {
        return 0;
    }
    int64_t v = 0;
    int m = 1;
    for (int idx = dd1; idx <= dd10; idx++) {
        v += m * digits[idx].decimal();
        m *= 10;
    }
    v = (digits[dd0].biq() == BiqDigit::biqMinus)? -v : v;
    return v;
}

void BiqWord::binary(int64_t v) {
    if (v > 9999999999LL || v < -9999999999LL) {
        return;
    }
    for (int idx = dd1; idx <= dd10; idx++) {
        digits[idx].biq(BiqDigit::biq0);
    }
    if (v < 0) {
        digits[dd0].biq(BiqDigit::biqMinus);
        v = -v;
    } else {
        digits[dd0].biq(BiqDigit::biqPlus);
    }
    int idx = dd1;
    while (v > 0) {
        digits[idx++].decimal(v % 10);
        v /= 10;
    }
}
    
int BiqWord::opCode() const {
    int v = 0;
    if (digits[dd10].isValid() && digits[dd9].isValid()) {
        v = digits[dd9].decimal() * 10
          + digits[dd10].decimal();
    }
    return v;
}
void BiqWord::opCode(int v) {
    v = (v < 0)? -v : v;
    v %= 50;
    digits[dd9].decimal(v % 10);
    digits[dd10].decimal(v / 10 % 10);
}
    
int BiqWord::iAddr() const {
    int v = 0;
    if (digits[dd8].isValid() && digits[dd7].isValid() &&
        digits[dd6].isValid() && digits[dd5].isValid()) {
        v = digits[dd8].decimal() * 1000
          + digits[dd7].decimal() * 100
          + digits[dd6].decimal() * 10
          + digits[dd5].decimal();
    }
    return v;
}

void BiqWord::iAddr(int v) {
    v = (v < 0)? -v : v;
    v %= 10000;
    for (int idx = dd5; idx <= dd8; idx++) {
        digits[idx].decimal(v % 10);
        v /= 10;
    }
}
    
int BiqWord::dAddr() const {
    int v = 0;
    if (digits[dd4].isValid() && digits[dd3].isValid() &&
        digits[dd2].isValid() && digits[dd1].isValid()) {
        v = digits[dd4].decimal() * 1000
          + digits[dd3].decimal() * 100
          + digits[dd2].decimal() * 10
          + digits[dd1].decimal();
    }
    return v;
}

void BiqWord::dAddr(int v) {
    v = (v < 0)? -v : v;
    v %= 10000;
    for (int idx = dd1; idx <= dd4; idx++) {
        digits[idx].decimal(v % 10);
        v /= 10;
    }
}
    
void BiqWord::print(ostream& s) const {
    for (auto biq : digits) {
        biq.print(s);
    }
}
void BiqWord::printReversed(ostream& s) const {
    s << digits[dd0].signCharacter();
    for (auto i = dd10; i >= dd1; i--) {
        s << digits[i];
    }
}

ostream& operator<<(ostream& s, const BiqWord& v) {
    v.print(s);
    return s;
}

void testBiqDigit() {
    auto bd = BiqDigit();
    bd.biq(4095);
    std::cout << "isValid: " << bd.isValid() << std::endl;
    bd.biq(BiqDigit::biq0);
    std::cout << "isValid: " << bd.isValid() << std::endl;
    std::cout << "decimal: " << bd.decimal() << std::endl;
    std::cout << "drumCode: " << bd.drumCode() << std::endl;
    bd.decimal(3);
    std::cout << "decimal: " << bd.decimal() << std::endl;
    std::cout << bd << std::endl;
    for (int i = 0; i < 10; i++) {
        bd.decimal(i);
        std::cout << "decimal: " << bd.decimal() << std::endl;
    }
}

void testBiqWord() {
    auto bw = BiqWord();
    std::cout << "bw: " << bw << std::endl;
    bw[BiqWord::dd0] = BiqDigit::biq7;
    std::cout << "bw: " << bw << std::endl;
    std::cout << "signedDecimal: " << bw.signedDecimal() << std::endl;
    bw.signedDecimal("-12345");
    std::cout << "signedDecimal: " << bw.signedDecimal() << std::endl;
    bw.signedDecimal("-0");
    std::cout << "signedDecimal: " << bw.signedDecimal() << std::endl;
}