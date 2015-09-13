//
//  biq.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/23/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef biq_hpp
#define biq_hpp

class BiqDigit {
  public:
    // Bi-quinary decimal digit codes
    static const int biq0 = 0b0100001;
    static const int biq1 = 0b0100010;
    static const int biq2 = 0b0100100;
    static const int biq3 = 0b0101000;
    static const int biq4 = 0b0110000;
    static const int biq5 = 0b1000001;
    static const int biq6 = 0b1000010;
    static const int biq7 = 0b1000100;
    static const int biq8 = 0b1001000;
    static const int biq9 = 0b1010000;
    static const int biqMinus = biq8;   // Signs must use these codes exclusively
    static const int biqPlus  = biq9;
    static const int biqBlank = 0;
    // Drum 2-of-5 decimal digit codes
    static const int drum0 = 0b01100;
    static const int drum1 = 0b11000;
    static const int drum2 = 0b10100;
    static const int drum3 = 0b10010;
    static const int drum4 = 0b01010;
    static const int drum5 = 0b00110;
    static const int drum6 = 0b10001;
    static const int drum7 = 0b01001;
    static const int drum8 = 0b00101;
    static const int drum9 = 0b00011;
    static const int drumBlank = 0;
    // Bi-quinary bit masks
    static const int biq_q0 = 0b0000001;
    static const int biq_q1 = 0b0000010;
    static const int biq_q2 = 0b0000100;
    static const int biq_q4 = 0b0001000;
    static const int biq_q5 = 0b0010000;
    static const int biq_b0 = 0b0100000;
    static const int biq_b5 = 0b1000000;
  private:
    int digitBits = biqBlank;
    
  public:
    BiqDigit() : digitBits(biqBlank) { }
    BiqDigit(int biq) : digitBits(biq & 0x7f) { }
    
    explicit operator int() const {return digitBits;}

    bool operator !=(BiqDigit rhs) const {
        return (digitBits != rhs.biq());
    }
    bool operator ==(BiqDigit rhs) const {
        return (digitBits == rhs.biq());
    }
    BiqDigit operator =(BiqDigit rhs) {
        digitBits = rhs.biq();
        return *this;
    }
    BiqDigit operator &(BiqDigit rhs) {
        digitBits &= rhs.biq();
        return *this;
    }
    
    bool isValid() const {
        switch (digitBits) {
            case biq0: case biq1: case biq2: case biq3: case biq4:
            case biq5: case biq6: case biq7: case biq8: case biq9:
                return true;
            default: return false;
        }
    }
    int biq() const { return digitBits; }
    void biq(int v) { digitBits = v & 0x7f; }
    int decimal() const {
        switch (digitBits) {
            case biq0: return 0;
            case biq1: return 1;
            case biq2: return 2;
            case biq3: return 3;
            case biq4: return 4;
            case biq5: return 5;
            case biq6: return 6;
            case biq7: return 7;
            case biq8: return 8;
            case biq9: return 9;
            default:   return 0;
        };
    }
    void decimal(int v) {
        switch (v) {
            case 0: digitBits = biq0; break;
            case 1: digitBits = biq1; break;
            case 2: digitBits = biq2; break;
            case 3: digitBits = biq3; break;
            case 4: digitBits = biq4; break;
            case 5: digitBits = biq5; break;
            case 6: digitBits = biq6; break;
            case 7: digitBits = biq7; break;
            case 8: digitBits = biq8; break;
            case 9: digitBits = biq9; break;
            default: break;
        };
    }
    int drumCode() const {
        switch (digitBits) {
            case biq0: return drum0;
            case biq1: return drum1;
            case biq2: return drum2;
            case biq3: return drum3;
            case biq4: return drum4;
            case biq5: return drum5;
            case biq6: return drum6;
            case biq7: return drum7;
            case biq8: return drum8;
            case biq9: return drum9;
            default:   return drumBlank;
        };
    }
    void drumCode(int v) {
        switch (v) {
            case drum0: digitBits = biq0; break;
            case drum1: digitBits = biq1; break;
            case drum2: digitBits = biq2; break;
            case drum3: digitBits = biq3; break;
            case drum4: digitBits = biq4; break;
            case drum5: digitBits = biq5; break;
            case drum6: digitBits = biq6; break;
            case drum7: digitBits = biq7; break;
            case drum8: digitBits = biq8; break;
            case drum9: digitBits = biq9; break;
            default:    digitBits = biqBlank;
        };
    }
    char character() const {
        switch (digitBits) {
            case biq0: return '0';
            case biq1: return '1';
            case biq2: return '2';
            case biq3: return '3';
            case biq4: return '4';
            case biq5: return '5';
            case biq6: return '6';
            case biq7: return '7';
            case biq8: return '8';
            case biq9: return '9';
            case biqBlank: return '_';
            default:   return 'X';
        };
    }
    char signCharacter() const {
        switch (digitBits) {
            case biq0: return 'v';
            case biq1: return 'v';
            case biq2: return 'v';
            case biq3: return 'v';
            case biq4: return 'v';
            case biq5: return 'v';
            case biq6: return 'v';
            case biq7: return 'v';
            case biqMinus: return '-';
            case biqPlus: return '+';
            case biqBlank: return '_';
            default:   return 'X';
        }
    }
    BiqDigit ninesComplement() const {
        switch (digitBits) {
            case biq0: return biq9;
            case biq1: return biq8;
            case biq2: return biq7;
            case biq3: return biq6;
            case biq4: return biq5;
            case biq5: return biq4;
            case biq6: return biq3;
            case biq7: return biq2;
            case biq8: return biq1;
            case biq9: return biq0;
            default:   return biqBlank;
        };
    }
    BiqDigit add(const BiqDigit& rhs, bool carryIn) const {
        if (isValid() && rhs.isValid()) {
            BiqDigit biqSum;
            biqSum.decimal((decimal() + rhs.decimal() + carryIn) % 10);
            return biqSum;
        } else {
            return biqBlank;
        }
    }
    bool carryFromAdd(const BiqDigit& rhs, bool carryIn) {
        if (isValid() && rhs.isValid()) {
            auto sum = decimal() + rhs.decimal() + carryIn;
            return (sum > 9);
        } else {
            return false;
        }
    }
    
    void print(std::ostream& s) const {
        s << character();
    }
};

std::ostream& operator<<(std::ostream& s, const BiqDigit& v);

class BiqWord {
  public:
    // digits in this order: Dx, D0...D10
    static const int ddx = 0;
    static const int dd0 = 1;
    static const int dd1 = 2;
    static const int dd2 = 3;
    static const int dd3 = 4;
    static const int dd4 = 5;
    static const int dd5 = 6;
    static const int dd6 = 7;
    static const int dd7 = 8;
    static const int dd8 = 9;
    static const int dd9 = 10;
    static const int dd10 = 11;
  private:
    std::vector<BiqDigit> digits;
  public:
    BiqWord() : digits(12) { }
    BiqWord(const std::string& d) : digits(12) {signedDecimal(d);}
    explicit BiqWord(long long v) : digits(12) {binary(v);}

    const BiqDigit& operator[](int i) const {
        return digits[i];
    }
    BiqDigit& operator[](int i) {
        return digits[i];
    }
    bool compare(const BiqWord& rhs) const {
        bool equal = true;
        for (auto i = 0; i < 12; i++) {
            if (digits[i] != rhs[i]) {
                equal = false;
                break;
            }
        }
        return equal;
    }
    bool operator==(const BiqWord& rhs) const {
        return compare(rhs);
    }
    bool operator!=(const BiqWord& rhs) const {
        return !compare(rhs);
    }
    
    bool isValid() const {
        bool v = true;
        if (digits[dd0].biq() != BiqDigit::biqPlus &&
            digits[dd0].biq() != BiqDigit::biqMinus) {
            v = false;
        }
        for (int idx = dd1; idx <= dd10; idx++) {
            if (!digits[idx].isValid()) {
                v = false;
            }
        }
        return v;
    }
    
    void blank() {
        for (int idx = ddx; idx <= dd10; idx++) {
            digits[idx].biq(BiqDigit::biqBlank);
        }
    }
    void plusZero() {
        digits[ddx].biq(BiqDigit::biqBlank);
        digits[dd0].biq(BiqDigit::biqPlus);
        for (int idx = dd1; idx <= dd10; idx++) {
            digits[idx].biq(BiqDigit::biq0);
        }
    }
    void minusZero() {
        digits[ddx].biq(BiqDigit::biqBlank);
        digits[dd0].biq(BiqDigit::biqMinus);
        for (int idx = dd1; idx <= dd10; idx++) {
            digits[idx].biq(BiqDigit::biq0);
        }
    }
    
    std::string signedDecimal() const;
    void signedDecimal(const std::string& v);
    int64_t binary() const;
    void binary(int64_t v);
    int opCode() const;
    void opCode(int v);
    int iAddr() const;
    void iAddr(int v);
    int dAddr() const;
    void dAddr(int v);
    void print(std::ostream& s) const;
    void printReversed(std::ostream& s) const;
};

std::ostream& operator<<(std::ostream& s, const BiqWord& v);

#endif /* biq_hpp */
