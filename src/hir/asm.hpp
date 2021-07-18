/*
 */
#pragma once
#include <tagged_union.hpp>

namespace AsmCommon {

    enum class Direction {
        In,
        Out,
        LateOut,
        InOut,
        InLateOut
    };
    static inline std::ostream& operator<<(std::ostream& os, const Direction& d) {
        switch(d)
        {
        case Direction::In:     return os << "in";
        case Direction::Out:    return os << "out";
        case Direction::LateOut:    return os << "lateout";
        case Direction::InOut:      return os << "inout";
        case Direction::InLateOut:  return os << "inlateout";
        }
        return os;
    }

    enum class RegisterClass {
        x86_reg,
        x86_reg_abcd,
        x86_reg_byte,
        x86_xmm,
        x86_ymm,
        x86_zmm,
        //x86_mm, // Requires
        x86_kreg,

        //aarch64_reg,
        //aarch64_vreg,
        
        //arm_reg,
        //arm_sreg,
        //arm_dreg,
        //arm_qreg,
        
        //mips_reg,
        //mips_freg,
        
        //nvptx_reg16,
        //nvptx_reg32,
        //nvptx_reg64,
        
        //riscv_reg,
        //riscv_freg,
        
        //hexagon_reg,
        
        //powerpc_reg,
        //powerpc_reg_nonzero,
        //powerpc_freg,
        
        //wasm32_local,
        
        //bpf_reg,
        //bpf_wreg,
    };

    TAGGED_UNION_EX(RegisterSpec, (), Explicit, (
        (Class, RegisterClass),
        (Explicit, std::string)
        ), (),(), (
            RegisterSpec clone() const {
                TU_MATCH_HDRA((*this),{)
                TU_ARMA(Class, e)   return e;
                TU_ARMA(Explicit, e)    return e;
                }
                throw "";
            }
        )
        );
    static inline std::ostream& operator<<(std::ostream& os, const RegisterSpec& s) {
        TU_MATCH_HDRA((s), {)
        TU_ARMA(Class, c) {
            switch(c)
            {
            case RegisterClass::x86_reg:    return os << "reg";
            case RegisterClass::x86_reg_abcd:   return os << "reg_abcd";
            case RegisterClass::x86_reg_byte:   return os << "reg_byte";
            case RegisterClass::x86_xmm:    return os << "xmm_reg";
            case RegisterClass::x86_ymm:    return os << "ymm_reg";
            case RegisterClass::x86_zmm:    return os << "zmm_reg";
            case RegisterClass::x86_kreg:   return os << "kreg";
            }
            }
        TU_ARMA(Explicit, e) {
            os << "\"" << e << "\"";
            }
        }
        return os;
    }

    struct LineFragment {
        std::string before;

        unsigned    index;
        char    modifier;
        
        LineFragment()
            : index(UINT_MAX)
            , modifier('\0')
        {
        }
    };
    struct Line {
        std::vector<LineFragment>   frags;
        std::string trailing;

        void fmt(std::ostream& os) const;
    };
    struct Options {
        unsigned pure : 1;
        unsigned nomem : 1;
        unsigned readonly : 1;
        unsigned preserves_flags : 1;
        unsigned noreturn : 1;
        unsigned nostack : 1;
        unsigned att_syntax : 1;
        Options()
            : pure(0)
            , nomem(0)
            , readonly(0)
            , preserves_flags(0)
            , noreturn(0)
            , nostack(0)
            , att_syntax(0)
        {
        }
        bool any() const {
            if(pure)    return true;
            if(nomem)   return true;
            if(readonly)    return true;
            if(preserves_flags) return true;
            if(noreturn)    return true;
            if(nostack) return true;
            if(att_syntax)  return true;
            return false;
        }
    };
}