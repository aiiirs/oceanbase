/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#include "lib/charset/ob_ctype_tis620_tab.h"
#include "lib/charset/ob_ctype_tis620.h"

const int t_ctype[][TOT_LEVELS] = {
    /*0x00*/ {0, 0, 0, 0, X},
    /*0x01*/ {0, 0, 0, 0, X},
    /*0x02*/ {0, 0, 0, 0, X},
    /*0x03*/ {0, 0, 0, 0, X},
    /*0x04*/ {0, 0, 0, 0, X},
    /*0x05*/ {0, 0, 0, 0, X},
    /*0x06*/ {0, 0, 0, 0, X},
    /*0x07*/ {0, 0, 0, 0, X},
    /*0x08*/ {0, 0, 0, 0, X},
    /*0x09*/ {0, 0, 0, 0, X},
    /*0x0A*/ {0, 0, 0, 0, X},
    /*0x0B*/ {0, 0, 0, 0, X},
    /*0x0C*/ {0, 0, 0, 0, X},
    /*0x0D*/ {0, 0, 0, 0, X},
    /*0x0E*/ {0, 0, 0, 0, X},
    /*0x0F*/ {0, 0, 0, 0, X},
    /*0x10*/ {0, 0, 0, 0, X},
    /*0x11*/ {0, 0, 0, 0, X},
    /*0x12*/ {0, 0, 0, 0, X},
    /*0x13*/ {0, 0, 0, 0, X},
    /*0x14*/ {0, 0, 0, 0, X},
    /*0x15*/ {0, 0, 0, 0, X},
    /*0x16*/ {0, 0, 0, 0, X},
    /*0x17*/ {0, 0, 0, 0, X},
    /*0x18*/ {0, 0, 0, 0, X},
    /*0x19*/ {0, 0, 0, 0, X},
    /*0x1A*/ {0, 0, 0, 0, X},
    /*0x1B*/ {0, 0, 0, 0, X},
    /*0x1C*/ {0, 0, 0, 0, X},
    /*0x1D*/ {0, 0, 0, 0, X},
    /*0x1E*/ {0, 0, 0, 0, X},
    /*0x1F*/ {0, 0, 0, 0, X},
    /*0x20*/ {0, 0, L3_SPACE, 0, M},
    /*0x21*/ {0, 0, L3_EXCLAMATION, 0, M},
    /*0x22*/ {0, 0, L3_QUOTATION, 0, M},
    /*0x23*/ {0, 0, L3_NUMBER, 0, M},
    /*0x24*/ {0, 0, L3_DOLLAR, 0, M},
    /*0x25*/ {0, 0, L3_PERCENT, 0, M},
    /*0x26*/ {0, 0, L3_AMPERSAND, 0, M},
    /*0x27*/ {0, 0, L3_APOSTROPHE, 0, M},
    /*0x28*/ {0, 0, L3_L_PARANTHESIS, 0, M},
    /*0x29*/ {0, 0, L3_R_PARENTHESIS, 0, M},
    /*0x2A*/ {0, 0, L3_ASTERISK, 0, M},
    /*0x2B*/ {0, 0, L3_PLUS, 0, M},
    /*0x2C*/ {0, 0, L3_COMMA, 0, M},
    /*0x2D*/ {0, 0, L3_HYPHEN, 0, M},
    /*0x2E*/ {0, 0, L3_FULL_STOP, 0, M},
    /*0x2F*/ {0, 0, L3_SOLIDUS, 0, M},
    /*0x30*/ {L1_08, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x31*/ {L1_18, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x32*/ {L1_28, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x33*/ {L1_38, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x34*/ {L1_48, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x35*/ {L1_58, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x36*/ {L1_68, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x37*/ {L1_78, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x38*/ {L1_88, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x39*/ {L1_98, L2_BLANK, L3_BLANK, L4_BLANK, M},
    /*0x3A*/ {0, 0, L3_COLON, 0, M},
    /*0x3B*/ {0, 0, L3_SEMICOLON, 0, M},
    /*0x3C*/ {0, 0, L3_LESS_THAN, 0, M},
    /*0x3D*/ {0, 0, L3_EQUAL, 0, M},
    /*0x3E*/ {0, 0, L3_GREATER_THAN, 0, M},
    /*0x3F*/ {0, 0, L3_QUESTION, 0, M},
    /*0x40*/ {0, 0, L3_AT, 0, M},
    /*0x41*/ {L1_A8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x42*/ {L1_B8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x43*/ {L1_C8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x44*/ {L1_D8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x45*/ {L1_E8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x46*/ {L1_F8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x47*/ {L1_G8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x48*/ {L1_H8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x49*/ {L1_I8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x4A*/ {L1_J8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x4B*/ {L1_K8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x4C*/ {L1_L8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x4D*/ {L1_M8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x4E*/ {L1_N8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x4F*/ {L1_O8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x50*/ {L1_P8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x51*/ {L1_Q8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x52*/ {L1_R8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x53*/ {L1_S8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x54*/ {L1_T8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x55*/ {L1_U8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x56*/ {L1_V8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x57*/ {L1_W8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x58*/ {L1_X8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x59*/ {L1_Y8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x5A*/ {L1_Z8, L2_BLANK, L3_BLANK, L4_CAP, M},
    /*0x5B*/ {0, 0, L3_L_BRACKET, 0, M},
    /*0x5C*/ {0, 0, L3_BK_SOLIDUS, 0, M},
    /*0x5D*/ {0, 0, L3_R_BRACKET, 0, M},
    /*0x5E*/ {0, 0, L3_CIRCUMFLEX, 0, M},
    /*0x5F*/ {0, 0, L3_LOW_LINE, 0, M},
    /*0x60*/ {0, 0, L3_GRAVE, 0, M},
    /*0x61*/ {L1_A8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x62*/ {L1_B8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x63*/ {L1_C8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x64*/ {L1_D8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x65*/ {L1_E8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x66*/ {L1_F8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x67*/ {L1_G8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x68*/ {L1_H8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x69*/ {L1_I8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x6A*/ {L1_J8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x6B*/ {L1_K8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x6C*/ {L1_L8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x6D*/ {L1_M8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x6E*/ {L1_N8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x6F*/ {L1_O8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x70*/ {L1_P8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x71*/ {L1_Q8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x72*/ {L1_R8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x73*/ {L1_S8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x74*/ {L1_T8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x75*/ {L1_U8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x76*/ {L1_V8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x77*/ {L1_W8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x78*/ {L1_X8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x79*/ {L1_Y8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x7A*/ {L1_Z8, L2_BLANK, L3_BLANK, L4_MIN, M},
    /*0x7B*/ {0, 0, L3_L_BRACE, 0, M},
    /*0x7C*/ {0, 0, L3_V_LINE, 0, M},
    /*0x7D*/ {0, 0, L3_R_BRACE, 0, M},
    /*0x7E*/ {0, 0, L3_TILDE, 0, M},
    /*0x7F*/ {0, 0, 0, 0, X},
    /*0x80*/ {0, 0, 0, 0, X},
    /*0x81*/ {0, 0, 0, 0, X},
    /*0x82*/ {0, 0, 0, 0, X},
    /*0x83*/ {0, 0, 0, 0, X},
    /*0x84*/ {0, 0, 0, 0, X},
    /*0x85*/ {0, 0, 0, 0, X},
    /*0x86*/ {0, 0, 0, 0, X},
    /*0x87*/ {0, 0, 0, 0, X},
    /*0x88*/ {0, 0, 0, 0, X},
    /*0x89*/ {0, 0, 0, 0, X},
    /*0x8A*/ {0, 0, 0, 0, X},
    /*0x8B*/ {0, 0, 0, 0, X},
    /*0x8C*/ {0, 0, 0, 0, X},
    /*0x8D*/ {0, 0, 0, 0, X},
    /*0x8E*/ {0, 0, 0, 0, X},
    /*0x8F*/ {0, 0, 0, 0, X},
    /*0x90*/ {0, 0, 0, 0, X},
    /*0x91*/ {0, 0, 0, 0, X},
    /*0x92*/ {0, 0, 0, 0, X},
    /*0x93*/ {0, 0, 0, 0, X},
    /*0x94*/ {0, 0, 0, 0, X},
    /*0x95*/ {0, 0, 0, 0, X},
    /*0x96*/ {0, 0, 0, 0, X},
    /*0x97*/ {0, 0, 0, 0, X},
    /*0x98*/ {0, 0, 0, 0, X},
    /*0x99*/ {0, 0, 0, 0, X},
    /*0x9A*/ {0, 0, 0, 0, X},
    /*0x9B*/ {0, 0, 0, 0, X},
    /*0x9C*/ {0, 0, 0, 0, X},
    /*0x9D*/ {0, 0, 0, 0, X},
    /*0x9E*/ {0, 0, 0, 0, X},
    /*0x9F*/ {0, 0, 0, 0, X},
    /*0xA0*/ {0, 0, L3_NB_SACE, 0, X},
    /*0xA1*/ {L1_KO_KAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA2*/ {L1_KHO_KHAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA3*/ {L1_KHO_KHUAT, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA4*/ {L1_KHO_KHWAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA5*/ {L1_KHO_KHON, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA6*/ {L1_KHO_RAKHANG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA7*/ {L1_NGO_NGU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA8*/ {L1_CHO_CHAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA9*/ {L1_CHO_CHING, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAA*/ {L1_CHO_CHANG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAB*/ {L1_SO_SO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAC*/ {L1_CHO_CHOE, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAD*/ {L1_YO_YING, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAE*/ {L1_DO_CHADA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAF*/ {L1_TO_PATAK, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB0*/ {L1_THO_THAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB1*/ {L1_THO_NANGMONTHO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB2*/ {L1_THO_PHUTHAO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB3*/ {L1_NO_NEN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB4*/ {L1_DO_DEK, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB5*/ {L1_TO_TAO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB6*/ {L1_THO_THUNG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB7*/ {L1_THO_THAHAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB8*/ {L1_THO_THONG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB9*/ {L1_NO_NU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBA*/ {L1_BO_BAIMAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBB*/ {L1_PO_PLA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBC*/ {L1_PHO_PHUNG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBD*/ {L1_FO_FA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBE*/ {L1_PHO_PHAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBF*/ {L1_FO_FAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC0*/ {L1_PHO_SAMPHAO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC1*/ {L1_MO_MA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC2*/ {L1_YO_YAK, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC3*/ {L1_RO_RUA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC4*/ {L1_RU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC5*/ {L1_LO_LING, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC6*/ {L1_LU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC7*/ {L1_WO_WAEN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC8*/ {L1_SO_SALA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC9*/ {L1_SO_RUSI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCA*/ {L1_SO_SUA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCB*/ {L1_HO_HIP, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCC*/ {L1_LO_CHULA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCD*/ {L1_O_ANG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCE*/ {L1_HO_NOKHUK, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCF*/ {0, 0, L3_PAIYAN_NOI, 0, M},
    /*0xD0*/ {L1_SARA_A, L2_BLANK, L3_BLANK, L4_BLANK, M | _fllwvowel},
    /*0xD1*/ {L1_MAI_HAN_AKAT, L2_BLANK, L3_BLANK, L4_BLANK, L_UPPER | _uprvowel},
    /*0xD2*/ {L1_SARA_AA, L2_BLANK, L3_BLANK, L4_BLANK, M | _fllwvowel},
    /*0xD3*/ {L1_SARA_AM, L2_BLANK, L3_BLANK, L4_BLANK, M | _fllwvowel},
    /*0xD4*/ {L1_SARA_I, L2_BLANK, L3_BLANK, L4_BLANK, L_UPPER | _uprvowel},
    /*0xD5*/ {L1_SARA_II, L2_BLANK, L3_BLANK, L4_BLANK, L_UPPER | _uprvowel},
    /*0xD6*/ {L1_SARA_UE, L2_BLANK, L3_BLANK, L4_BLANK, L_UPPER | _uprvowel},
    /*0xD7*/ {L1_SARA_UEE, L2_BLANK, L3_BLANK, L4_BLANK, L_UPPER | _uprvowel},
    /*0xD8*/ {L1_SARA_U, L2_BLANK, L3_BLANK, L4_BLANK, L | _lwrvowel},
    /*0xD9*/ {L1_SARA_UU, L2_BLANK, L3_BLANK, L4_BLANK, L | _lwrvowel},
    /*0xDA*/ {0, L2_PINTHU, L3_BLANK, L4_BLANK, L},
    /*0xDB*/ {0, 0, 0, 0, X},
    /*0xDC*/ {0, 0, 0, 0, X},
    /*0xDD*/ {0, 0, 0, 0, X},
    /*0xDE*/ {0, 0, 0, 0, X},
    /*0xDF*/ {0, 0, L3_BAHT, 0, M},
    /*0xE0*/ {L1_SARA_E, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel},
    /*0xE1*/ {L1_SARA_AE, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel},
    /*0xE2*/ {L1_SARA_O, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel},
    /*0xE3*/ {L1_SARA_AI_MAIMUAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel},
    /*0xE4*/ {L1_SARA_AI_MAIMALAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel},
    /*0xE5*/ {L1_SARA_AA, L2_BLANK, L3_BLANK, L4_EXT, M | _fllwvowel},
    /*0xE6*/ {0, 0, L3_MAI_YAMOK, 0, M | _stone},
    /*0xE7*/ {0, L2_TYKHU, L3_BLANK, L4_BLANK, L_UPPER | _diacrt1 | _stone},
    /*0xE8*/
    {0, L2_TONE1, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone},
    /*0xE9*/
    {0, L2_TONE2, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone},
    /*0xEA*/
    {0, L2_TONE3, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone},
    /*0xEB*/
    {0, L2_TONE4, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone},
    /*0xEC*/
    {0, L2_GARAN, L3_BLANK, L4_BLANK, UU | _diacrt2 | _combine | _stone},
    /*0xED*/ {L1_NKHIT, L2_BLANK, L3_BLANK, L4_BLANK, L_UPPER | _diacrt1},
    /*0xEE*/ {0, L2_YAMAK, L3_BLANK, L4_BLANK, L_UPPER | _diacrt1},
    /*0xEF*/ {0, 0, L3_FONGMAN, 0, M},
    /*0xF0*/ {L1_08, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF1*/ {L1_18, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF2*/ {L1_28, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF3*/ {L1_38, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF4*/ {L1_48, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF5*/ {L1_58, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF6*/ {L1_68, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF7*/ {L1_78, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF8*/ {L1_88, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xF9*/ {L1_98, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig},
    /*0xFA*/ {0, 0, L3_ANGKHANKHU, 0, X},
    /*0xFB*/ {0, 0, L3_KHOMUT, 0, X},
    /*0xFC*/ {0, 0, 0, 0, X},
    /*0xFD*/ {0, 0, 0, 0, X},
    /*0xFE*/ {0, 0, 0, 0, X},
    /* Utilize 0xFF for max_sort_chr */
    /*0xFF*/ {255 /*0*/, 0, 0, 0, X},
};

unsigned char ctype_tis620[257] = {
    0, /* For standard library */
    32,  32,  32,  32,  32,  32,  32,  32,  32,  40,  40, 40, 40, 40, 32, 32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32, 32, 32, 32, 32, 32,
    72,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16, 16, 16, 16, 16, 16,
    132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 16, 16, 16, 16, 16, 16,
    16,  129, 129, 129, 129, 129, 129, 1,   1,   1,   1,  1,  1,  1,  1,  1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,  16, 16, 16, 16, 16,
    16,  130, 130, 130, 130, 130, 130, 2,   2,   2,   2,  2,  2,  2,  2,  2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  16, 16, 16, 16, 32,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,
};

unsigned char to_lower_tis620[] = {
    '\000',        '\001',        '\002',        '\003',        '\004',
    '\005',        '\006',        '\007',        '\010',        '\011',
    '\012',        '\013',        '\014',        '\015',        '\016',
    '\017',        '\020',        '\021',        '\022',        '\023',
    '\024',        '\025',        '\026',        '\027',        '\030',
    '\031',        '\032',        '\033',        '\034',        '\035',
    '\036',        '\037',        ' ',           '!',           '"',
    '#',           '$',           '%',           '&',           '\'',
    '(',           ')',           '*',           '+',           ',',
    '-',           '.',           '/',           '0',           '1',
    '2',           '3',           '4',           '5',           '6',
    '7',           '8',           '9',           ':',           ';',
    '<',           '=',           '>',           '?',           '@',
    'a',           'b',           'c',           'd',           'e',
    'f',           'g',           'h',           'i',           'j',
    'k',           'l',           'm',           'n',           'o',
    'p',           'q',           'r',           's',           't',
    'u',           'v',           'w',           'x',           'y',
    'z',           '[',           '\\',          ']',           '^',
    '_',           '`',           'a',           'b',           'c',
    'd',           'e',           'f',           'g',           'h',
    'i',           'j',           'k',           'l',           'm',
    'n',           'o',           'p',           'q',           'r',
    's',           't',           'u',           'v',           'w',
    'x',           'y',           'z',           '{',           '|',
    '}',           '~',           '\177',        (uchar)'\200', (uchar)'\201',
    (uchar)'\202', (uchar)'\203', (uchar)'\204', (uchar)'\205', (uchar)'\206',
    (uchar)'\207', (uchar)'\210', (uchar)'\211', (uchar)'\212', (uchar)'\213',
    (uchar)'\214', (uchar)'\215', (uchar)'\216', (uchar)'\217', (uchar)'\220',
    (uchar)'\221', (uchar)'\222', (uchar)'\223', (uchar)'\224', (uchar)'\225',
    (uchar)'\226', (uchar)'\227', (uchar)'\230', (uchar)'\231', (uchar)'\232',
    (uchar)'\233', (uchar)'\234', (uchar)'\235', (uchar)'\236', (uchar)'\237',
    (uchar)'\240', (uchar)'\241', (uchar)'\242', (uchar)'\243', (uchar)'\244',
    (uchar)'\245', (uchar)'\246', (uchar)'\247', (uchar)'\250', (uchar)'\251',
    (uchar)'\252', (uchar)'\253', (uchar)'\254', (uchar)'\255', (uchar)'\256',
    (uchar)'\257', (uchar)'\260', (uchar)'\261', (uchar)'\262', (uchar)'\263',
    (uchar)'\264', (uchar)'\265', (uchar)'\266', (uchar)'\267', (uchar)'\270',
    (uchar)'\271', (uchar)'\272', (uchar)'\273', (uchar)'\274', (uchar)'\275',
    (uchar)'\276', (uchar)'\277', (uchar)'\300', (uchar)'\301', (uchar)'\302',
    (uchar)'\303', (uchar)'\304', (uchar)'\305', (uchar)'\306', (uchar)'\307',
    (uchar)'\310', (uchar)'\311', (uchar)'\312', (uchar)'\313', (uchar)'\314',
    (uchar)'\315', (uchar)'\316', (uchar)'\317', (uchar)'\320', (uchar)'\321',
    (uchar)'\322', (uchar)'\323', (uchar)'\324', (uchar)'\325', (uchar)'\326',
    (uchar)'\327', (uchar)'\330', (uchar)'\331', (uchar)'\332', (uchar)'\333',
    (uchar)'\334', (uchar)'\335', (uchar)'\336', (uchar)'\337', (uchar)'\340',
    (uchar)'\341', (uchar)'\342', (uchar)'\343', (uchar)'\344', (uchar)'\345',
    (uchar)'\346', (uchar)'\347', (uchar)'\350', (uchar)'\351', (uchar)'\352',
    (uchar)'\353', (uchar)'\354', (uchar)'\355', (uchar)'\356', (uchar)'\357',
    (uchar)'\360', (uchar)'\361', (uchar)'\362', (uchar)'\363', (uchar)'\364',
    (uchar)'\365', (uchar)'\366', (uchar)'\367', (uchar)'\370', (uchar)'\371',
    (uchar)'\372', (uchar)'\373', (uchar)'\374', (uchar)'\375', (uchar)'\376',
    (uchar)'\377',
};

unsigned char to_upper_tis620[] = {
    '\000',        '\001',        '\002',        '\003',        '\004',
    '\005',        '\006',        '\007',        '\010',        '\011',
    '\012',        '\013',        '\014',        '\015',        '\016',
    '\017',        '\020',        '\021',        '\022',        '\023',
    '\024',        '\025',        '\026',        '\027',        '\030',
    '\031',        '\032',        '\033',        '\034',        '\035',
    '\036',        '\037',        ' ',           '!',           '"',
    '#',           '$',           '%',           '&',           '\'',
    '(',           ')',           '*',           '+',           ',',
    '-',           '.',           '/',           '0',           '1',
    '2',           '3',           '4',           '5',           '6',
    '7',           '8',           '9',           ':',           ';',
    '<',           '=',           '>',           '?',           '@',
    'A',           'B',           'C',           'D',           'E',
    'F',           'G',           'H',           'I',           'J',
    'K',           'L',           'M',           'N',           'O',
    'P',           'Q',           'R',           'S',           'T',
    'U',           'V',           'W',           'X',           'Y',
    'Z',           '[',           '\\',          ']',           '^',
    '_',           '`',           'A',           'B',           'C',
    'D',           'E',           'F',           'G',           'H',
    'I',           'J',           'K',           'L',           'M',
    'N',           'O',           'P',           'Q',           'R',
    'S',           'T',           'U',           'V',           'W',
    'X',           'Y',           'Z',           '{',           '|',
    '}',           '~',           '\177',        (uchar)'\200', (uchar)'\201',
    (uchar)'\202', (uchar)'\203', (uchar)'\204', (uchar)'\205', (uchar)'\206',
    (uchar)'\207', (uchar)'\210', (uchar)'\211', (uchar)'\212', (uchar)'\213',
    (uchar)'\214', (uchar)'\215', (uchar)'\216', (uchar)'\217', (uchar)'\220',
    (uchar)'\221', (uchar)'\222', (uchar)'\223', (uchar)'\224', (uchar)'\225',
    (uchar)'\226', (uchar)'\227', (uchar)'\230', (uchar)'\231', (uchar)'\232',
    (uchar)'\233', (uchar)'\234', (uchar)'\235', (uchar)'\236', (uchar)'\237',
    (uchar)'\240', (uchar)'\241', (uchar)'\242', (uchar)'\243', (uchar)'\244',
    (uchar)'\245', (uchar)'\246', (uchar)'\247', (uchar)'\250', (uchar)'\251',
    (uchar)'\252', (uchar)'\253', (uchar)'\254', (uchar)'\255', (uchar)'\256',
    (uchar)'\257', (uchar)'\260', (uchar)'\261', (uchar)'\262', (uchar)'\263',
    (uchar)'\264', (uchar)'\265', (uchar)'\266', (uchar)'\267', (uchar)'\270',
    (uchar)'\271', (uchar)'\272', (uchar)'\273', (uchar)'\274', (uchar)'\275',
    (uchar)'\276', (uchar)'\277', (uchar)'\300', (uchar)'\301', (uchar)'\302',
    (uchar)'\303', (uchar)'\304', (uchar)'\305', (uchar)'\306', (uchar)'\307',
    (uchar)'\310', (uchar)'\311', (uchar)'\312', (uchar)'\313', (uchar)'\314',
    (uchar)'\315', (uchar)'\316', (uchar)'\317', (uchar)'\320', (uchar)'\321',
    (uchar)'\322', (uchar)'\323', (uchar)'\324', (uchar)'\325', (uchar)'\326',
    (uchar)'\327', (uchar)'\330', (uchar)'\331', (uchar)'\332', (uchar)'\333',
    (uchar)'\334', (uchar)'\335', (uchar)'\336', (uchar)'\337', (uchar)'\340',
    (uchar)'\341', (uchar)'\342', (uchar)'\343', (uchar)'\344', (uchar)'\345',
    (uchar)'\346', (uchar)'\347', (uchar)'\350', (uchar)'\351', (uchar)'\352',
    (uchar)'\353', (uchar)'\354', (uchar)'\355', (uchar)'\356', (uchar)'\357',
    (uchar)'\360', (uchar)'\361', (uchar)'\362', (uchar)'\363', (uchar)'\364',
    (uchar)'\365', (uchar)'\366', (uchar)'\367', (uchar)'\370', (uchar)'\371',
    (uchar)'\372', (uchar)'\373', (uchar)'\374', (uchar)'\375', (uchar)'\376',
    (uchar)'\377',
};

unsigned char sort_order_tis620[] = {
    '\000',        '\001',        '\002',        '\003',        '\004',
    '\005',        '\006',        '\007',        '\010',        '\011',
    '\012',        '\013',        '\014',        '\015',        '\016',
    '\017',        '\020',        '\021',        '\022',        '\023',
    '\024',        '\025',        '\026',        '\027',        '\030',
    '\031',        '\032',        '\033',        '\034',        '\035',
    '\036',        '\037',        ' ',           '!',           '"',
    '#',           '$',           '%',           '&',           '\'',
    '(',           ')',           '*',           '+',           ',',
    '-',           '.',           '/',           '0',           '1',
    '2',           '3',           '4',           '5',           '6',
    '7',           '8',           '9',           ':',           ';',
    '<',           '=',           '>',           '?',           '@',
    'A',           'B',           'C',           'D',           'E',
    'F',           'G',           'H',           'I',           'J',
    'K',           'L',           'M',           'N',           'O',
    'P',           'Q',           'R',           'S',           'T',
    'U',           'V',           'W',           'X',           'Y',
    'Z',           '\\',          ']',           '[',           '^',
    '_',           'E',           'A',           'B',           'C',
    'D',           'E',           'F',           'G',           'H',
    'I',           'J',           'K',           'L',           'M',
    'N',           'O',           'P',           'Q',           'R',
    'S',           'T',           'U',           'V',           'W',
    'X',           'Y',           'Z',           '{',           '|',
    '}',           'Y',           '\177',        (uchar)'\200', (uchar)'\201',
    (uchar)'\202', (uchar)'\203', (uchar)'\204', (uchar)'\205', (uchar)'\206',
    (uchar)'\207', (uchar)'\210', (uchar)'\211', (uchar)'\212', (uchar)'\213',
    (uchar)'\214', (uchar)'\215', (uchar)'\216', (uchar)'\217', (uchar)'\220',
    (uchar)'\221', (uchar)'\222', (uchar)'\223', (uchar)'\224', (uchar)'\225',
    (uchar)'\226', (uchar)'\227', (uchar)'\230', (uchar)'\231', (uchar)'\232',
    (uchar)'\233', (uchar)'\234', (uchar)'\235', (uchar)'\236', (uchar)'\237',
    (uchar)'\240', (uchar)'\241', (uchar)'\242', (uchar)'\243', (uchar)'\244',
    (uchar)'\245', (uchar)'\246', (uchar)'\247', (uchar)'\250', (uchar)'\251',
    (uchar)'\252', (uchar)'\253', (uchar)'\254', (uchar)'\255', (uchar)'\256',
    (uchar)'\257', (uchar)'\260', (uchar)'\261', (uchar)'\262', (uchar)'\263',
    (uchar)'\264', (uchar)'\265', (uchar)'\266', (uchar)'\267', (uchar)'\270',
    (uchar)'\271', (uchar)'\272', (uchar)'\273', (uchar)'\274', (uchar)'\275',
    (uchar)'\276', (uchar)'\277', (uchar)'\300', (uchar)'\301', (uchar)'\302',
    (uchar)'\303', (uchar)'\304', (uchar)'\305', (uchar)'\306', (uchar)'\307',
    (uchar)'\310', (uchar)'\311', (uchar)'\312', (uchar)'\313', (uchar)'\314',
    (uchar)'\315', (uchar)'\316', (uchar)'\317', (uchar)'\320', (uchar)'\321',
    (uchar)'\322', (uchar)'\323', (uchar)'\324', (uchar)'\325', (uchar)'\326',
    (uchar)'\327', (uchar)'\330', (uchar)'\331', (uchar)'\332', (uchar)'\333',
    (uchar)'\334', (uchar)'\335', (uchar)'\336', (uchar)'\337', (uchar)'\340',
    (uchar)'\341', (uchar)'\342', (uchar)'\343', (uchar)'\344', (uchar)'\345',
    (uchar)'\346', (uchar)'\347', (uchar)'\350', (uchar)'\351', (uchar)'\352',
    (uchar)'\353', (uchar)'\354', (uchar)'\355', (uchar)'\356', (uchar)'\357',
    (uchar)'\360', (uchar)'\361', (uchar)'\362', (uchar)'\363', (uchar)'\364',
    (uchar)'\365', (uchar)'\366', (uchar)'\367', (uchar)'\370', (uchar)'\371',
    (uchar)'\372', (uchar)'\373', (uchar)'\374', (uchar)'\375', (uchar)'\376',
    (uchar)'\377',
};


unsigned short cs_to_uni_tis620[256] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
    0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011,
    0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,
    0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023,
    0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C,
    0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
    0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E,
    0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050,
    0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
    0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062,
    0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B,
    0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
    0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D,
    0x007E, 0x007F, 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086,
    0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098,
    0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F, 0xFFFD, 0x0E01,
    0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07, 0x0E08, 0x0E09, 0x0E0A,
    0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F, 0x0E10, 0x0E11, 0x0E12, 0x0E13,
    0x0E14, 0x0E15, 0x0E16, 0x0E17, 0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C,
    0x0E1D, 0x0E1E, 0x0E1F, 0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25,
    0x0E26, 0x0E27, 0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E,
    0x0E2F, 0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37,
    0x0E38, 0x0E39, 0x0E3A, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x0E3F, 0x0E40,
    0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47, 0x0E48, 0x0E49,
    0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F, 0x0E50, 0x0E51, 0x0E52,
    0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57, 0x0E58, 0x0E59, 0x0E5A, 0x0E5B,
    0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD};
unsigned char pl00_tis620[256] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
    0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011,
    0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A,
    0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023,
    0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C,
    0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035,
    0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E,
    0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050,
    0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
    0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062,
    0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B,
    0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
    0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D,
    0x007E, 0x007F, 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086,
    0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098,
    0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000};
unsigned char pl0E[256] = {
    0x0000, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8,
    0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 0x00B0, 0x00B1,
    0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA,
    0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3,
    0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC,
    0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5,
    0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x0000, 0x0000, 0x0000, 0x0000,
    0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F0,
    0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9,
    0x00FA, 0x00FB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000};
unsigned char plFF[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x00FF, 0x0000, 0x0000};
unsigned char *uni_to_cs_tis620[256] = {
    pl00_tis620,    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, pl0E,    NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, plFF};