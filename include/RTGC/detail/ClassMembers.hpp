#ifndef RTGC_DETAIL_CLASSMEMBERS_HPP
#define RTGC_DETAIL_CLASSMEMBERS_HPP

#include<type_traits>
#include<utility>

namespace RTGC { namespace detail {

#define X_1 x1
#define X_2 X_1,x2
#define X_3 X_2,x3
#define X_4 X_3,x4
#define X_5 X_4,x5
#define X_6 X_5,x6
#define X_7 X_6,x7
#define X_8 X_7,x8
#define X_9 X_8,x9
#define X_10 X_9,x10

#define X_11 X_10,x11
#define X_12 X_11,x12
#define X_13 X_12,x13
#define X_14 X_13,x14
#define X_15 X_14,x15
#define X_16 X_15,x16
#define X_17 X_16,x17
#define X_18 X_17,x18
#define X_19 X_18,x19
#define X_20 X_19,x20

#define X_21 X_20,x21
#define X_22 X_21,x22
#define X_23 X_22,x23
#define X_24 X_23,x24
#define X_25 X_24,x25
#define X_26 X_25,x26
#define X_27 X_26,x27
#define X_28 X_27,x28
#define X_29 X_28,x29
#define X_30 X_29,x30

#define X_31 X_30,x31
#define X_32 X_31,x32
#define X_33 X_32,x33
#define X_34 X_33,x34
#define X_35 X_34,x35
#define X_36 X_35,x36
#define X_37 X_36,x37
#define X_38 X_37,x38
#define X_39 X_38,x39
#define X_40 X_39,x40

#define X_41 X_40,x41
#define X_42 X_41,x42
#define X_43 X_42,x43
#define X_44 X_43,x44
#define X_45 X_44,x45
#define X_46 X_45,x46
#define X_47 X_46,x47
#define X_48 X_47,x48
#define X_49 X_48,x49
#define X_50 X_49,x50

#define X_51 X_50,x51
#define X_52 X_51,x52
#define X_53 X_52,x53
#define X_54 X_53,x54
#define X_55 X_54,x55
#define X_56 X_55,x56
#define X_57 X_56,x57
#define X_58 X_57,x58
#define X_59 X_58,x59
#define X_60 X_59,x60

#define X_61 X_60,x61
#define X_62 X_61,x62
#define X_63 X_62,x63
#define X_64 X_63,x64
#define X_65 X_64,x65
#define X_66 X_65,x66
#define X_67 X_66,x67
#define X_68 X_67,x68
#define X_69 X_68,x69
#define X_70 X_69,x70

#define X_71 X_70,x71
#define X_72 X_71,x72
#define X_73 X_72,x73
#define X_74 X_73,x74
#define X_75 X_74,x75
#define X_76 X_75,x76
#define X_77 X_76,x77
#define X_78 X_77,x78
#define X_79 X_78,x79
#define X_80 X_79,x80

#define X_81 X_80,x81
#define X_82 X_81,x82
#define X_83 X_82,x83
#define X_84 X_83,x84
#define X_85 X_84,x85
#define X_86 X_85,x86
#define X_87 X_86,x87
#define X_88 X_87,x88
#define X_89 X_88,x89
#define X_90 X_89,x90

#define X_91 X_90,x91
#define X_92 X_91,x92
#define X_93 X_92,x93
#define X_94 X_93,x94
#define X_95 X_94,x95
#define X_96 X_95,x96
#define X_97 X_96,x97
#define X_98 X_97,x98
#define X_99 X_98,x99
#define X_100 X_99,x100


template <std::size_t N>
using const_size_t = std::integral_constant<std::size_t, N>;

#define CONST_SIZE(num) const_size_t<(num)>

template <class... Args>
constexpr auto MakeConstexprTupleByReferences(Args&&... args) noexcept {
    return std::tuple<Args&...>{args... };
}

#define DEF_TIE_FUNC(num)                                   \
constexpr static auto TieToTuple(T& val, CONST_SIZE(num)) { \
    auto &[X_##num] = val;                                  \
    return MakeConstexprTupleByReferences(X_##num);         \
}

template <class T>
struct ToTuple{
    DEF_TIE_FUNC(1);
    DEF_TIE_FUNC(2);
    DEF_TIE_FUNC(3);
    DEF_TIE_FUNC(4);
    DEF_TIE_FUNC(5);
    DEF_TIE_FUNC(6);
    DEF_TIE_FUNC(7);
    DEF_TIE_FUNC(8);
    DEF_TIE_FUNC(9);
    DEF_TIE_FUNC(10);
    
    DEF_TIE_FUNC(11);
    DEF_TIE_FUNC(12);
    DEF_TIE_FUNC(13);
    DEF_TIE_FUNC(14);
    DEF_TIE_FUNC(15);
    DEF_TIE_FUNC(16);
    DEF_TIE_FUNC(17);
    DEF_TIE_FUNC(18);
    DEF_TIE_FUNC(19);
    DEF_TIE_FUNC(20);
    
    DEF_TIE_FUNC(21);
    DEF_TIE_FUNC(22);
    DEF_TIE_FUNC(23);
    DEF_TIE_FUNC(24);
    DEF_TIE_FUNC(25);
    DEF_TIE_FUNC(26);
    DEF_TIE_FUNC(27);
    DEF_TIE_FUNC(28);
    DEF_TIE_FUNC(29);
    DEF_TIE_FUNC(30);
    
    DEF_TIE_FUNC(31);
    DEF_TIE_FUNC(32);
    DEF_TIE_FUNC(33);
    DEF_TIE_FUNC(34);
    DEF_TIE_FUNC(35);
    DEF_TIE_FUNC(36);
    DEF_TIE_FUNC(37);
    DEF_TIE_FUNC(38);
    DEF_TIE_FUNC(39);
    DEF_TIE_FUNC(40);
    
    DEF_TIE_FUNC(41);
    DEF_TIE_FUNC(42);
    DEF_TIE_FUNC(43);
    DEF_TIE_FUNC(44);
    DEF_TIE_FUNC(45);
    DEF_TIE_FUNC(46);
    DEF_TIE_FUNC(47);
    DEF_TIE_FUNC(48);
    DEF_TIE_FUNC(49);
    DEF_TIE_FUNC(50);
    
    DEF_TIE_FUNC(51);
    DEF_TIE_FUNC(52);
    DEF_TIE_FUNC(53);
    DEF_TIE_FUNC(54);
    DEF_TIE_FUNC(55);
    DEF_TIE_FUNC(56);
    DEF_TIE_FUNC(57);
    DEF_TIE_FUNC(58);
    DEF_TIE_FUNC(59);
    DEF_TIE_FUNC(60);
    
    DEF_TIE_FUNC(61);
    DEF_TIE_FUNC(62);
    DEF_TIE_FUNC(63);
    DEF_TIE_FUNC(64);
    DEF_TIE_FUNC(65);
    DEF_TIE_FUNC(66);
    DEF_TIE_FUNC(67);
    DEF_TIE_FUNC(68);
    DEF_TIE_FUNC(69);
    DEF_TIE_FUNC(70);
    
    DEF_TIE_FUNC(71);
    DEF_TIE_FUNC(72);
    DEF_TIE_FUNC(73);
    DEF_TIE_FUNC(74);
    DEF_TIE_FUNC(75);
    DEF_TIE_FUNC(76);
    DEF_TIE_FUNC(77);
    DEF_TIE_FUNC(78);
    DEF_TIE_FUNC(79);
    DEF_TIE_FUNC(80);
    
    DEF_TIE_FUNC(81);
    DEF_TIE_FUNC(82);
    DEF_TIE_FUNC(83);
    DEF_TIE_FUNC(84);
    DEF_TIE_FUNC(85);
    DEF_TIE_FUNC(86);
    DEF_TIE_FUNC(87);
    DEF_TIE_FUNC(88);
    DEF_TIE_FUNC(89);
    DEF_TIE_FUNC(90);
    
    DEF_TIE_FUNC(91);
    DEF_TIE_FUNC(92);
    DEF_TIE_FUNC(93);
    DEF_TIE_FUNC(94);
    DEF_TIE_FUNC(95);
    DEF_TIE_FUNC(96);
    DEF_TIE_FUNC(97);
    DEF_TIE_FUNC(98);
    DEF_TIE_FUNC(99);
    DEF_TIE_FUNC(100);
    template <std::size_t N>
    constexpr static auto TieToTuple(T& val, const_size_t<N>) {
        static_assert(N != N, "====================> RTGC: add TieToTuple(T& val, const_size_t<N>)");
    }
};

}}

#endif
