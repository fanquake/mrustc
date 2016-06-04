/*
 */
#pragma once

#include <memory>
#include <vector>
#include <tagged_union.hpp>
#include <hir/path.hpp>
#include <hir/type.hpp>

namespace HIR {

class Struct;
class Enum;

struct PatternBinding
{
    enum class Type {
        Move,
        Ref,
        MutRef,
    };
    
    bool    m_mutable;
    Type    m_type;
    ::std::string   m_name;
    unsigned int    m_slot;
    
    bool is_valid() const { return m_name == ""; }
    
    PatternBinding():
        m_mutable(false),
        m_type(Type::Move),
        m_name(""),
        m_slot(0)
    {}
    PatternBinding(bool mut, Type type, ::std::string name, unsigned int slot):
        m_mutable(mut),
        m_type(type),
        m_name( mv$(name) ),
        m_slot( slot )
    {}
};

struct Pattern
{
    TAGGED_UNION(Value, String,
        (Integer, struct {
            enum ::HIR::CoreType type;
            uint64_t value; // Signed numbers are encoded as 2's complement
            }),
        (String, ::std::string),
        (Named, Path)
        );
    friend ::std::ostream& operator<<(::std::ostream& os, const Pattern::Value& x);

    TAGGED_UNION(Data, Any,
        // Irrefutable / destructuring
        (Any,       struct { } ),
        (Box,       struct { ::std::unique_ptr<Pattern> sub; }),
        (Ref,       struct { ::HIR::BorrowType type; ::std::unique_ptr<Pattern> sub; } ),
        (Tuple,     struct { ::std::vector<Pattern> sub_patterns; } ),
        (StructTuple, struct {
            // NOTE: Type paths in patterns _can_ have parameters
            GenericPath path;
            const Struct*   binding;
            ::std::vector<Pattern> sub_patterns;
            } ),
        (StructTupleWildcard, struct {
            GenericPath path;
            const Struct*   binding;
            }),
        (Struct,    struct {
            GenericPath path;
            const Struct*   binding;
            ::std::vector< ::std::pair< ::std::string, Pattern> > sub_patterns;
            bool is_exhaustive;
            } ),
        // Refutable
        (Value,     struct { Value val; } ),
        (Range,     struct { Value start; Value end; } ),
        (EnumTuple, struct {
            GenericPath path;
            const Enum* binding_ptr;
            unsigned binding_idx;
            ::std::vector<Pattern> sub_patterns;
            } ),
        (EnumTupleWildcard, struct {
            GenericPath path;
            const Enum* binding_ptr;
            unsigned binding_idx;
            } ),
        (EnumStruct, struct {
            GenericPath path;
            const Enum* binding_ptr;
            unsigned binding_idx;
            ::std::vector< ::std::pair< ::std::string, Pattern> > sub_patterns;
            bool is_exhaustive;
            } ),
        (Slice,     struct {
            ::std::vector<Pattern> sub_patterns;
            } ),
        (SplitSlice, struct {
            ::std::vector<Pattern> leading;
            PatternBinding extra_bind;
            ::std::vector<Pattern> trailing;
            } )
        );

    PatternBinding  m_binding;
    Data    m_data;
    
    friend ::std::ostream& operator<<(::std::ostream& os, const Pattern& x);
};

}

