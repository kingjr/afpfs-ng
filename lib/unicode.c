/**********************************************************************
 *
 * unicode.c: Functions to handle UTF8/UCS2 coded strings.
 *
 * Most of these functions have been adopted from Roland Krause's
 * UTF8.c, which is part of the XawPlus package. See
 * http://freenet-homepage.de/kra/ for details.
 *
 * int str16len()       A strlen() on a char16 string
 * char16 *str16chr()   A strchr() on a char16 string
 * void str16cpy()      A strcpy() on a char16 string
 * void str16ncpy()     A strncpy() on a char16 string
 * void str16cat()      A strcat() on a char16 string
 *
 * int mbCharLen()      Calc number of byte of the UTF8 character
 * int mbStrLen()       Calc # of characters in UTF8 string
 * char16 *UTF8toUCS2() Convert UTF8 string to UCS2/UNICODE
 * char *UCS2toUTF8()   Convert UCS2/UNICODE string to UTF8
 *
 * int UCS2precompose() Canonically combine two UCS2 characters
 *      
 * Copyright (c) Roland Krause 2002, roland_krause@freenet.de 
 * Copyright (c) Michael Ulbrich 2007, mul@rentapacs.de
 *      
 * This module is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **********************************************************************/

#include <stdlib.h>
#include "unicode.h"

// Size of table: N = 997

static struct {
  int precomposed;
  unsigned int pattern;
} table[] = {
{ 0x0000, 0x00000000},    // Dummy entry table[0]
{ 0x00C0, 0x00410300},
{ 0x00C1, 0x00410301},
{ 0x00C2, 0x00410302},
{ 0x00C3, 0x00410303},
{ 0x0100, 0x00410304},
{ 0x0102, 0x00410306},
{ 0x0226, 0x00410307},
{ 0x00C4, 0x00410308},
{ 0x1EA2, 0x00410309},
{ 0x00C5, 0x0041030A},
{ 0x01CD, 0x0041030C},
{ 0x0200, 0x0041030F},
{ 0x0202, 0x00410311},
{ 0x1EA0, 0x00410323},
{ 0x1E00, 0x00410325},
{ 0x0104, 0x00410328},
{ 0x1E02, 0x00420307},
{ 0x1E04, 0x00420323},
{ 0x1E06, 0x00420331},
{ 0x0106, 0x00430301},
{ 0x0108, 0x00430302},
{ 0x010A, 0x00430307},
{ 0x010C, 0x0043030C},
{ 0x00C7, 0x00430327},
{ 0x1E0A, 0x00440307},
{ 0x010E, 0x0044030C},
{ 0x1E0C, 0x00440323},
{ 0x1E10, 0x00440327},
{ 0x1E12, 0x0044032D},
{ 0x1E0E, 0x00440331},
{ 0x00C8, 0x00450300},
{ 0x00C9, 0x00450301},
{ 0x00CA, 0x00450302},
{ 0x1EBC, 0x00450303},
{ 0x0112, 0x00450304},
{ 0x0114, 0x00450306},
{ 0x0116, 0x00450307},
{ 0x00CB, 0x00450308},
{ 0x1EBA, 0x00450309},
{ 0x011A, 0x0045030C},
{ 0x0204, 0x0045030F},
{ 0x0206, 0x00450311},
{ 0x1EB8, 0x00450323},
{ 0x0228, 0x00450327},
{ 0x0118, 0x00450328},
{ 0x1E18, 0x0045032D},
{ 0x1E1A, 0x00450330},
{ 0x1E1E, 0x00460307},
{ 0x01F4, 0x00470301},
{ 0x011C, 0x00470302},
{ 0x1E20, 0x00470304},
{ 0x011E, 0x00470306},
{ 0x0120, 0x00470307},
{ 0x01E6, 0x0047030C},
{ 0x0122, 0x00470327},
{ 0x0124, 0x00480302},
{ 0x1E22, 0x00480307},
{ 0x1E26, 0x00480308},
{ 0x021E, 0x0048030C},
{ 0x1E24, 0x00480323},
{ 0x1E28, 0x00480327},
{ 0x1E2A, 0x0048032E},
{ 0x00CC, 0x00490300},
{ 0x00CD, 0x00490301},
{ 0x00CE, 0x00490302},
{ 0x0128, 0x00490303},
{ 0x012A, 0x00490304},
{ 0x012C, 0x00490306},
{ 0x0130, 0x00490307},
{ 0x00CF, 0x00490308},
{ 0x1EC8, 0x00490309},
{ 0x01CF, 0x0049030C},
{ 0x0208, 0x0049030F},
{ 0x020A, 0x00490311},
{ 0x1ECA, 0x00490323},
{ 0x012E, 0x00490328},
{ 0x1E2C, 0x00490330},
{ 0x0134, 0x004A0302},
{ 0x1E30, 0x004B0301},
{ 0x01E8, 0x004B030C},
{ 0x1E32, 0x004B0323},
{ 0x0136, 0x004B0327},
{ 0x1E34, 0x004B0331},
{ 0x0139, 0x004C0301},
{ 0x013D, 0x004C030C},
{ 0x1E36, 0x004C0323},
{ 0x013B, 0x004C0327},
{ 0x1E3C, 0x004C032D},
{ 0x1E3A, 0x004C0331},
{ 0x1E3E, 0x004D0301},
{ 0x1E40, 0x004D0307},
{ 0x1E42, 0x004D0323},
{ 0x01F8, 0x004E0300},
{ 0x0143, 0x004E0301},
{ 0x00D1, 0x004E0303},
{ 0x1E44, 0x004E0307},
{ 0x0147, 0x004E030C},
{ 0x1E46, 0x004E0323},
{ 0x0145, 0x004E0327},
{ 0x1E4A, 0x004E032D},
{ 0x1E48, 0x004E0331},
{ 0x00D2, 0x004F0300},
{ 0x00D3, 0x004F0301},
{ 0x00D4, 0x004F0302},
{ 0x00D5, 0x004F0303},
{ 0x014C, 0x004F0304},
{ 0x014E, 0x004F0306},
{ 0x022E, 0x004F0307},
{ 0x00D6, 0x004F0308},
{ 0x1ECE, 0x004F0309},
{ 0x0150, 0x004F030B},
{ 0x01D1, 0x004F030C},
{ 0x020C, 0x004F030F},
{ 0x020E, 0x004F0311},
{ 0x01A0, 0x004F031B},
{ 0x1ECC, 0x004F0323},
{ 0x01EA, 0x004F0328},
{ 0x1E54, 0x00500301},
{ 0x1E56, 0x00500307},
{ 0x0154, 0x00520301},
{ 0x1E58, 0x00520307},
{ 0x0158, 0x0052030C},
{ 0x0210, 0x0052030F},
{ 0x0212, 0x00520311},
{ 0x1E5A, 0x00520323},
{ 0x0156, 0x00520327},
{ 0x1E5E, 0x00520331},
{ 0x015A, 0x00530301},
{ 0x015C, 0x00530302},
{ 0x1E60, 0x00530307},
{ 0x0160, 0x0053030C},
{ 0x1E62, 0x00530323},
{ 0x0218, 0x00530326},
{ 0x015E, 0x00530327},
{ 0x1E6A, 0x00540307},
{ 0x0164, 0x0054030C},
{ 0x1E6C, 0x00540323},
{ 0x021A, 0x00540326},
{ 0x0162, 0x00540327},
{ 0x1E70, 0x0054032D},
{ 0x1E6E, 0x00540331},
{ 0x00D9, 0x00550300},
{ 0x00DA, 0x00550301},
{ 0x00DB, 0x00550302},
{ 0x0168, 0x00550303},
{ 0x016A, 0x00550304},
{ 0x016C, 0x00550306},
{ 0x00DC, 0x00550308},
{ 0x1EE6, 0x00550309},
{ 0x016E, 0x0055030A},
{ 0x0170, 0x0055030B},
{ 0x01D3, 0x0055030C},
{ 0x0214, 0x0055030F},
{ 0x0216, 0x00550311},
{ 0x01AF, 0x0055031B},
{ 0x1EE4, 0x00550323},
{ 0x1E72, 0x00550324},
{ 0x0172, 0x00550328},
{ 0x1E76, 0x0055032D},
{ 0x1E74, 0x00550330},
{ 0x1E7C, 0x00560303},
{ 0x1E7E, 0x00560323},
{ 0x1E80, 0x00570300},
{ 0x1E82, 0x00570301},
{ 0x0174, 0x00570302},
{ 0x1E86, 0x00570307},
{ 0x1E84, 0x00570308},
{ 0x1E88, 0x00570323},
{ 0x1E8A, 0x00580307},
{ 0x1E8C, 0x00580308},
{ 0x1EF2, 0x00590300},
{ 0x00DD, 0x00590301},
{ 0x0176, 0x00590302},
{ 0x1EF8, 0x00590303},
{ 0x0232, 0x00590304},
{ 0x1E8E, 0x00590307},
{ 0x0178, 0x00590308},
{ 0x1EF6, 0x00590309},
{ 0x1EF4, 0x00590323},
{ 0x0179, 0x005A0301},
{ 0x1E90, 0x005A0302},
{ 0x017B, 0x005A0307},
{ 0x017D, 0x005A030C},
{ 0x1E92, 0x005A0323},
{ 0x1E94, 0x005A0331},
{ 0x00E0, 0x00610300},
{ 0x00E1, 0x00610301},
{ 0x00E2, 0x00610302},
{ 0x00E3, 0x00610303},
{ 0x0101, 0x00610304},
{ 0x0103, 0x00610306},
{ 0x0227, 0x00610307},
{ 0x00E4, 0x00610308},
{ 0x1EA3, 0x00610309},
{ 0x00E5, 0x0061030A},
{ 0x01CE, 0x0061030C},
{ 0x0201, 0x0061030F},
{ 0x0203, 0x00610311},
{ 0x1EA1, 0x00610323},
{ 0x1E01, 0x00610325},
{ 0x0105, 0x00610328},
{ 0x1E03, 0x00620307},
{ 0x1E05, 0x00620323},
{ 0x1E07, 0x00620331},
{ 0x0107, 0x00630301},
{ 0x0109, 0x00630302},
{ 0x010B, 0x00630307},
{ 0x010D, 0x0063030C},
{ 0x00E7, 0x00630327},
{ 0x1E0B, 0x00640307},
{ 0x010F, 0x0064030C},
{ 0x1E0D, 0x00640323},
{ 0x1E11, 0x00640327},
{ 0x1E13, 0x0064032D},
{ 0x1E0F, 0x00640331},
{ 0x00E8, 0x00650300},
{ 0x00E9, 0x00650301},
{ 0x00EA, 0x00650302},
{ 0x1EBD, 0x00650303},
{ 0x0113, 0x00650304},
{ 0x0115, 0x00650306},
{ 0x0117, 0x00650307},
{ 0x00EB, 0x00650308},
{ 0x1EBB, 0x00650309},
{ 0x011B, 0x0065030C},
{ 0x0205, 0x0065030F},
{ 0x0207, 0x00650311},
{ 0x1EB9, 0x00650323},
{ 0x0229, 0x00650327},
{ 0x0119, 0x00650328},
{ 0x1E19, 0x0065032D},
{ 0x1E1B, 0x00650330},
{ 0x1E1F, 0x00660307},
{ 0x01F5, 0x00670301},
{ 0x011D, 0x00670302},
{ 0x1E21, 0x00670304},
{ 0x011F, 0x00670306},
{ 0x0121, 0x00670307},
{ 0x01E7, 0x0067030C},
{ 0x0123, 0x00670327},
{ 0x0125, 0x00680302},
{ 0x1E23, 0x00680307},
{ 0x1E27, 0x00680308},
{ 0x021F, 0x0068030C},
{ 0x1E25, 0x00680323},
{ 0x1E29, 0x00680327},
{ 0x1E2B, 0x0068032E},
{ 0x1E96, 0x00680331},
{ 0x00EC, 0x00690300},
{ 0x00ED, 0x00690301},
{ 0x00EE, 0x00690302},
{ 0x0129, 0x00690303},
{ 0x012B, 0x00690304},
{ 0x012D, 0x00690306},
{ 0x00EF, 0x00690308},
{ 0x1EC9, 0x00690309},
{ 0x01D0, 0x0069030C},
{ 0x0209, 0x0069030F},
{ 0x020B, 0x00690311},
{ 0x1ECB, 0x00690323},
{ 0x012F, 0x00690328},
{ 0x1E2D, 0x00690330},
{ 0x0135, 0x006A0302},
{ 0x01F0, 0x006A030C},
{ 0x1E31, 0x006B0301},
{ 0x01E9, 0x006B030C},
{ 0x1E33, 0x006B0323},
{ 0x0137, 0x006B0327},
{ 0x1E35, 0x006B0331},
{ 0x013A, 0x006C0301},
{ 0x013E, 0x006C030C},
{ 0x1E37, 0x006C0323},
{ 0x013C, 0x006C0327},
{ 0x1E3D, 0x006C032D},
{ 0x1E3B, 0x006C0331},
{ 0x1E3F, 0x006D0301},
{ 0x1E41, 0x006D0307},
{ 0x1E43, 0x006D0323},
{ 0x01F9, 0x006E0300},
{ 0x0144, 0x006E0301},
{ 0x00F1, 0x006E0303},
{ 0x1E45, 0x006E0307},
{ 0x0148, 0x006E030C},
{ 0x1E47, 0x006E0323},
{ 0x0146, 0x006E0327},
{ 0x1E4B, 0x006E032D},
{ 0x1E49, 0x006E0331},
{ 0x00F2, 0x006F0300},
{ 0x00F3, 0x006F0301},
{ 0x00F4, 0x006F0302},
{ 0x00F5, 0x006F0303},
{ 0x014D, 0x006F0304},
{ 0x014F, 0x006F0306},
{ 0x022F, 0x006F0307},
{ 0x00F6, 0x006F0308},
{ 0x1ECF, 0x006F0309},
{ 0x0151, 0x006F030B},
{ 0x01D2, 0x006F030C},
{ 0x020D, 0x006F030F},
{ 0x020F, 0x006F0311},
{ 0x01A1, 0x006F031B},
{ 0x1ECD, 0x006F0323},
{ 0x01EB, 0x006F0328},
{ 0x1E55, 0x00700301},
{ 0x1E57, 0x00700307},
{ 0x0155, 0x00720301},
{ 0x1E59, 0x00720307},
{ 0x0159, 0x0072030C},
{ 0x0211, 0x0072030F},
{ 0x0213, 0x00720311},
{ 0x1E5B, 0x00720323},
{ 0x0157, 0x00720327},
{ 0x1E5F, 0x00720331},
{ 0x015B, 0x00730301},
{ 0x015D, 0x00730302},
{ 0x1E61, 0x00730307},
{ 0x0161, 0x0073030C},
{ 0x1E63, 0x00730323},
{ 0x0219, 0x00730326},
{ 0x015F, 0x00730327},
{ 0x1E6B, 0x00740307},
{ 0x1E97, 0x00740308},
{ 0x0165, 0x0074030C},
{ 0x1E6D, 0x00740323},
{ 0x021B, 0x00740326},
{ 0x0163, 0x00740327},
{ 0x1E71, 0x0074032D},
{ 0x1E6F, 0x00740331},
{ 0x00F9, 0x00750300},
{ 0x00FA, 0x00750301},
{ 0x00FB, 0x00750302},
{ 0x0169, 0x00750303},
{ 0x016B, 0x00750304},
{ 0x016D, 0x00750306},
{ 0x00FC, 0x00750308},
{ 0x1EE7, 0x00750309},
{ 0x016F, 0x0075030A},
{ 0x0171, 0x0075030B},
{ 0x01D4, 0x0075030C},
{ 0x0215, 0x0075030F},
{ 0x0217, 0x00750311},
{ 0x01B0, 0x0075031B},
{ 0x1EE5, 0x00750323},
{ 0x1E73, 0x00750324},
{ 0x0173, 0x00750328},
{ 0x1E77, 0x0075032D},
{ 0x1E75, 0x00750330},
{ 0x1E7D, 0x00760303},
{ 0x1E7F, 0x00760323},
{ 0x1E81, 0x00770300},
{ 0x1E83, 0x00770301},
{ 0x0175, 0x00770302},
{ 0x1E87, 0x00770307},
{ 0x1E85, 0x00770308},
{ 0x1E98, 0x0077030A},
{ 0x1E89, 0x00770323},
{ 0x1E8B, 0x00780307},
{ 0x1E8D, 0x00780308},
{ 0x1EF3, 0x00790300},
{ 0x00FD, 0x00790301},
{ 0x0177, 0x00790302},
{ 0x1EF9, 0x00790303},
{ 0x0233, 0x00790304},
{ 0x1E8F, 0x00790307},
{ 0x00FF, 0x00790308},
{ 0x1EF7, 0x00790309},
{ 0x1E99, 0x0079030A},
{ 0x1EF5, 0x00790323},
{ 0x017A, 0x007A0301},
{ 0x1E91, 0x007A0302},
{ 0x017C, 0x007A0307},
{ 0x017E, 0x007A030C},
{ 0x1E93, 0x007A0323},
{ 0x1E95, 0x007A0331},
{ 0x1FED, 0x00A80300},
{ 0x0385, 0x00A80301},
{ 0x1FC1, 0x00A80342},
{ 0x1EA6, 0x00C20300},
{ 0x1EA4, 0x00C20301},
{ 0x1EAA, 0x00C20303},
{ 0x1EA8, 0x00C20309},
{ 0x01DE, 0x00C40304},
{ 0x01FA, 0x00C50301},
{ 0x01FC, 0x00C60301},
{ 0x01E2, 0x00C60304},
{ 0x1E08, 0x00C70301},
{ 0x1EC0, 0x00CA0300},
{ 0x1EBE, 0x00CA0301},
{ 0x1EC4, 0x00CA0303},
{ 0x1EC2, 0x00CA0309},
{ 0x1E2E, 0x00CF0301},
{ 0x1ED2, 0x00D40300},
{ 0x1ED0, 0x00D40301},
{ 0x1ED6, 0x00D40303},
{ 0x1ED4, 0x00D40309},
{ 0x1E4C, 0x00D50301},
{ 0x022C, 0x00D50304},
{ 0x1E4E, 0x00D50308},
{ 0x022A, 0x00D60304},
{ 0x01FE, 0x00D80301},
{ 0x01DB, 0x00DC0300},
{ 0x01D7, 0x00DC0301},
{ 0x01D5, 0x00DC0304},
{ 0x01D9, 0x00DC030C},
{ 0x1EA7, 0x00E20300},
{ 0x1EA5, 0x00E20301},
{ 0x1EAB, 0x00E20303},
{ 0x1EA9, 0x00E20309},
{ 0x01DF, 0x00E40304},
{ 0x01FB, 0x00E50301},
{ 0x01FD, 0x00E60301},
{ 0x01E3, 0x00E60304},
{ 0x1E09, 0x00E70301},
{ 0x1EC1, 0x00EA0300},
{ 0x1EBF, 0x00EA0301},
{ 0x1EC5, 0x00EA0303},
{ 0x1EC3, 0x00EA0309},
{ 0x1E2F, 0x00EF0301},
{ 0x1ED3, 0x00F40300},
{ 0x1ED1, 0x00F40301},
{ 0x1ED7, 0x00F40303},
{ 0x1ED5, 0x00F40309},
{ 0x1E4D, 0x00F50301},
{ 0x022D, 0x00F50304},
{ 0x1E4F, 0x00F50308},
{ 0x022B, 0x00F60304},
{ 0x01FF, 0x00F80301},
{ 0x01DC, 0x00FC0300},
{ 0x01D8, 0x00FC0301},
{ 0x01D6, 0x00FC0304},
{ 0x01DA, 0x00FC030C},
{ 0x1EB0, 0x01020300},
{ 0x1EAE, 0x01020301},
{ 0x1EB4, 0x01020303},
{ 0x1EB2, 0x01020309},
{ 0x1EB1, 0x01030300},
{ 0x1EAF, 0x01030301},
{ 0x1EB5, 0x01030303},
{ 0x1EB3, 0x01030309},
{ 0x1E14, 0x01120300},
{ 0x1E16, 0x01120301},
{ 0x1E15, 0x01130300},
{ 0x1E17, 0x01130301},
{ 0x1E50, 0x014C0300},
{ 0x1E52, 0x014C0301},
{ 0x1E51, 0x014D0300},
{ 0x1E53, 0x014D0301},
{ 0x1E64, 0x015A0307},
{ 0x1E65, 0x015B0307},
{ 0x1E66, 0x01600307},
{ 0x1E67, 0x01610307},
{ 0x1E78, 0x01680301},
{ 0x1E79, 0x01690301},
{ 0x1E7A, 0x016A0308},
{ 0x1E7B, 0x016B0308},
{ 0x1E9B, 0x017F0307},
{ 0x1EDC, 0x01A00300},
{ 0x1EDA, 0x01A00301},
{ 0x1EE0, 0x01A00303},
{ 0x1EDE, 0x01A00309},
{ 0x1EE2, 0x01A00323},
{ 0x1EDD, 0x01A10300},
{ 0x1EDB, 0x01A10301},
{ 0x1EE1, 0x01A10303},
{ 0x1EDF, 0x01A10309},
{ 0x1EE3, 0x01A10323},
{ 0x1EEA, 0x01AF0300},
{ 0x1EE8, 0x01AF0301},
{ 0x1EEE, 0x01AF0303},
{ 0x1EEC, 0x01AF0309},
{ 0x1EF0, 0x01AF0323},
{ 0x1EEB, 0x01B00300},
{ 0x1EE9, 0x01B00301},
{ 0x1EEF, 0x01B00303},
{ 0x1EED, 0x01B00309},
{ 0x1EF1, 0x01B00323},
{ 0x01EE, 0x01B7030C},
{ 0x01EC, 0x01EA0304},
{ 0x01ED, 0x01EB0304},
{ 0x01E0, 0x02260304},
{ 0x01E1, 0x02270304},
{ 0x1E1C, 0x02280306},
{ 0x1E1D, 0x02290306},
{ 0x0230, 0x022E0304},
{ 0x0231, 0x022F0304},
{ 0x01EF, 0x0292030C},
{ 0x0344, 0x03080301},
{ 0x1FBA, 0x03910300},
{ 0x0386, 0x03910301},
{ 0x1FB9, 0x03910304},
{ 0x1FB8, 0x03910306},
{ 0x1F08, 0x03910313},
{ 0x1F09, 0x03910314},
{ 0x1FBC, 0x03910345},
{ 0x1FC8, 0x03950300},
{ 0x0388, 0x03950301},
{ 0x1F18, 0x03950313},
{ 0x1F19, 0x03950314},
{ 0x1FCA, 0x03970300},
{ 0x0389, 0x03970301},
{ 0x1F28, 0x03970313},
{ 0x1F29, 0x03970314},
{ 0x1FCC, 0x03970345},
{ 0x1FDA, 0x03990300},
{ 0x038A, 0x03990301},
{ 0x1FD9, 0x03990304},
{ 0x1FD8, 0x03990306},
{ 0x03AA, 0x03990308},
{ 0x1F38, 0x03990313},
{ 0x1F39, 0x03990314},
{ 0x1FF8, 0x039F0300},
{ 0x038C, 0x039F0301},
{ 0x1F48, 0x039F0313},
{ 0x1F49, 0x039F0314},
{ 0x1FEC, 0x03A10314},
{ 0x1FEA, 0x03A50300},
{ 0x038E, 0x03A50301},
{ 0x1FE9, 0x03A50304},
{ 0x1FE8, 0x03A50306},
{ 0x03AB, 0x03A50308},
{ 0x1F59, 0x03A50314},
{ 0x1FFA, 0x03A90300},
{ 0x038F, 0x03A90301},
{ 0x1F68, 0x03A90313},
{ 0x1F69, 0x03A90314},
{ 0x1FFC, 0x03A90345},
{ 0x1FB4, 0x03AC0345},
{ 0x1FC4, 0x03AE0345},
{ 0x1F70, 0x03B10300},
{ 0x03AC, 0x03B10301},
{ 0x1FB1, 0x03B10304},
{ 0x1FB0, 0x03B10306},
{ 0x1F00, 0x03B10313},
{ 0x1F01, 0x03B10314},
{ 0x1FB6, 0x03B10342},
{ 0x1FB3, 0x03B10345},
{ 0x1F72, 0x03B50300},
{ 0x03AD, 0x03B50301},
{ 0x1F10, 0x03B50313},
{ 0x1F11, 0x03B50314},
{ 0x1F74, 0x03B70300},
{ 0x03AE, 0x03B70301},
{ 0x1F20, 0x03B70313},
{ 0x1F21, 0x03B70314},
{ 0x1FC6, 0x03B70342},
{ 0x1FC3, 0x03B70345},
{ 0x1F76, 0x03B90300},
{ 0x03AF, 0x03B90301},
{ 0x1FD1, 0x03B90304},
{ 0x1FD0, 0x03B90306},
{ 0x03CA, 0x03B90308},
{ 0x1F30, 0x03B90313},
{ 0x1F31, 0x03B90314},
{ 0x1FD6, 0x03B90342},
{ 0x1F78, 0x03BF0300},
{ 0x03CC, 0x03BF0301},
{ 0x1F40, 0x03BF0313},
{ 0x1F41, 0x03BF0314},
{ 0x1FE4, 0x03C10313},
{ 0x1FE5, 0x03C10314},
{ 0x1F7A, 0x03C50300},
{ 0x03CD, 0x03C50301},
{ 0x1FE1, 0x03C50304},
{ 0x1FE0, 0x03C50306},
{ 0x03CB, 0x03C50308},
{ 0x1F50, 0x03C50313},
{ 0x1F51, 0x03C50314},
{ 0x1FE6, 0x03C50342},
{ 0x1F7C, 0x03C90300},
{ 0x03CE, 0x03C90301},
{ 0x1F60, 0x03C90313},
{ 0x1F61, 0x03C90314},
{ 0x1FF6, 0x03C90342},
{ 0x1FF3, 0x03C90345},
{ 0x1FD2, 0x03CA0300},
{ 0x0390, 0x03CA0301},
{ 0x1FD7, 0x03CA0342},
{ 0x1FE2, 0x03CB0300},
{ 0x03B0, 0x03CB0301},
{ 0x1FE7, 0x03CB0342},
{ 0x1FF4, 0x03CE0345},
{ 0x03D3, 0x03D20301},
{ 0x03D4, 0x03D20308},
{ 0x0407, 0x04060308},
{ 0x04D0, 0x04100306},
{ 0x04D2, 0x04100308},
{ 0x0403, 0x04130301},
{ 0x0400, 0x04150300},
{ 0x04D6, 0x04150306},
{ 0x0401, 0x04150308},
{ 0x04C1, 0x04160306},
{ 0x04DC, 0x04160308},
{ 0x04DE, 0x04170308},
{ 0x040D, 0x04180300},
{ 0x04E2, 0x04180304},
{ 0x0419, 0x04180306},
{ 0x04E4, 0x04180308},
{ 0x040C, 0x041A0301},
{ 0x04E6, 0x041E0308},
{ 0x04EE, 0x04230304},
{ 0x040E, 0x04230306},
{ 0x04F0, 0x04230308},
{ 0x04F2, 0x0423030B},
{ 0x04F4, 0x04270308},
{ 0x04F8, 0x042B0308},
{ 0x04EC, 0x042D0308},
{ 0x04D1, 0x04300306},
{ 0x04D3, 0x04300308},
{ 0x0453, 0x04330301},
{ 0x0450, 0x04350300},
{ 0x04D7, 0x04350306},
{ 0x0451, 0x04350308},
{ 0x04C2, 0x04360306},
{ 0x04DD, 0x04360308},
{ 0x04DF, 0x04370308},
{ 0x045D, 0x04380300},
{ 0x04E3, 0x04380304},
{ 0x0439, 0x04380306},
{ 0x04E5, 0x04380308},
{ 0x045C, 0x043A0301},
{ 0x04E7, 0x043E0308},
{ 0x04EF, 0x04430304},
{ 0x045E, 0x04430306},
{ 0x04F1, 0x04430308},
{ 0x04F3, 0x0443030B},
{ 0x04F5, 0x04470308},
{ 0x04F9, 0x044B0308},
{ 0x04ED, 0x044D0308},
{ 0x0457, 0x04560308},
{ 0x0476, 0x0474030F},
{ 0x0477, 0x0475030F},
{ 0x04DA, 0x04D80308},
{ 0x04DB, 0x04D90308},
{ 0x04EA, 0x04E80308},
{ 0x04EB, 0x04E90308},
{ 0xFB2E, 0x05D005B7},
{ 0xFB2F, 0x05D005B8},
{ 0xFB30, 0x05D005BC},
{ 0xFB31, 0x05D105BC},
{ 0xFB4C, 0x05D105BF},
{ 0xFB32, 0x05D205BC},
{ 0xFB33, 0x05D305BC},
{ 0xFB34, 0x05D405BC},
{ 0xFB4B, 0x05D505B9},
{ 0xFB35, 0x05D505BC},
{ 0xFB36, 0x05D605BC},
{ 0xFB38, 0x05D805BC},
{ 0xFB1D, 0x05D905B4},
{ 0xFB39, 0x05D905BC},
{ 0xFB3A, 0x05DA05BC},
{ 0xFB3B, 0x05DB05BC},
{ 0xFB4D, 0x05DB05BF},
{ 0xFB3C, 0x05DC05BC},
{ 0xFB3E, 0x05DE05BC},
{ 0xFB40, 0x05E005BC},
{ 0xFB41, 0x05E105BC},
{ 0xFB43, 0x05E305BC},
{ 0xFB44, 0x05E405BC},
{ 0xFB4E, 0x05E405BF},
{ 0xFB46, 0x05E605BC},
{ 0xFB47, 0x05E705BC},
{ 0xFB48, 0x05E805BC},
{ 0xFB49, 0x05E905BC},
{ 0xFB2A, 0x05E905C1},
{ 0xFB2B, 0x05E905C2},
{ 0xFB4A, 0x05EA05BC},
{ 0xFB1F, 0x05F205B7},
{ 0x0622, 0x06270653},
{ 0x0623, 0x06270654},
{ 0x0625, 0x06270655},
{ 0x0624, 0x06480654},
{ 0x0626, 0x064A0654},
{ 0x06C2, 0x06C10654},
{ 0x06D3, 0x06D20654},
{ 0x06C0, 0x06D50654},
{ 0x0958, 0x0915093C},
{ 0x0959, 0x0916093C},
{ 0x095A, 0x0917093C},
{ 0x095B, 0x091C093C},
{ 0x095C, 0x0921093C},
{ 0x095D, 0x0922093C},
{ 0x0929, 0x0928093C},
{ 0x095E, 0x092B093C},
{ 0x095F, 0x092F093C},
{ 0x0931, 0x0930093C},
{ 0x0934, 0x0933093C},
{ 0x09DC, 0x09A109BC},
{ 0x09DD, 0x09A209BC},
{ 0x09DF, 0x09AF09BC},
{ 0x09CB, 0x09C709BE},
{ 0x09CC, 0x09C709D7},
{ 0x0A59, 0x0A160A3C},
{ 0x0A5A, 0x0A170A3C},
{ 0x0A5B, 0x0A1C0A3C},
{ 0x0A5E, 0x0A2B0A3C},
{ 0x0A33, 0x0A320A3C},
{ 0x0A36, 0x0A380A3C},
{ 0x0B5C, 0x0B210B3C},
{ 0x0B5D, 0x0B220B3C},
{ 0x0B4B, 0x0B470B3E},
{ 0x0B48, 0x0B470B56},
{ 0x0B4C, 0x0B470B57},
{ 0x0B94, 0x0B920BD7},
{ 0x0BCA, 0x0BC60BBE},
{ 0x0BCC, 0x0BC60BD7},
{ 0x0BCB, 0x0BC70BBE},
{ 0x0C48, 0x0C460C56},
{ 0x0CC0, 0x0CBF0CD5},
{ 0x0CCA, 0x0CC60CC2},
{ 0x0CC7, 0x0CC60CD5},
{ 0x0CC8, 0x0CC60CD6},
{ 0x0CCB, 0x0CCA0CD5},
{ 0x0D4A, 0x0D460D3E},
{ 0x0D4C, 0x0D460D57},
{ 0x0D4B, 0x0D470D3E},
{ 0x0DDA, 0x0DD90DCA},
{ 0x0DDC, 0x0DD90DCF},
{ 0x0DDE, 0x0DD90DDF},
{ 0x0DDD, 0x0DDC0DCA},
{ 0x0F69, 0x0F400FB5},
{ 0x0F43, 0x0F420FB7},
{ 0x0F4D, 0x0F4C0FB7},
{ 0x0F52, 0x0F510FB7},
{ 0x0F57, 0x0F560FB7},
{ 0x0F5C, 0x0F5B0FB7},
{ 0x0F73, 0x0F710F72},
{ 0x0F75, 0x0F710F74},
{ 0x0F81, 0x0F710F80},
{ 0x0FB9, 0x0F900FB5},
{ 0x0F93, 0x0F920FB7},
{ 0x0F9D, 0x0F9C0FB7},
{ 0x0FA2, 0x0FA10FB7},
{ 0x0FA7, 0x0FA60FB7},
{ 0x0FAC, 0x0FAB0FB7},
{ 0x0F76, 0x0FB20F80},
{ 0x0F78, 0x0FB30F80},
{ 0x1026, 0x1025102E},
{ 0x1B06, 0x1B051B35},
{ 0x1B08, 0x1B071B35},
{ 0x1B0A, 0x1B091B35},
{ 0x1B0C, 0x1B0B1B35},
{ 0x1B0E, 0x1B0D1B35},
{ 0x1B12, 0x1B111B35},
{ 0x1B3B, 0x1B3A1B35},
{ 0x1B3D, 0x1B3C1B35},
{ 0x1B40, 0x1B3E1B35},
{ 0x1B41, 0x1B3F1B35},
{ 0x1B43, 0x1B421B35},
{ 0x1E38, 0x1E360304},
{ 0x1E39, 0x1E370304},
{ 0x1E5C, 0x1E5A0304},
{ 0x1E5D, 0x1E5B0304},
{ 0x1E68, 0x1E620307},
{ 0x1E69, 0x1E630307},
{ 0x1EAC, 0x1EA00302},
{ 0x1EB6, 0x1EA00306},
{ 0x1EAD, 0x1EA10302},
{ 0x1EB7, 0x1EA10306},
{ 0x1EC6, 0x1EB80302},
{ 0x1EC7, 0x1EB90302},
{ 0x1ED8, 0x1ECC0302},
{ 0x1ED9, 0x1ECD0302},
{ 0x1F02, 0x1F000300},
{ 0x1F04, 0x1F000301},
{ 0x1F06, 0x1F000342},
{ 0x1F80, 0x1F000345},
{ 0x1F03, 0x1F010300},
{ 0x1F05, 0x1F010301},
{ 0x1F07, 0x1F010342},
{ 0x1F81, 0x1F010345},
{ 0x1F82, 0x1F020345},
{ 0x1F83, 0x1F030345},
{ 0x1F84, 0x1F040345},
{ 0x1F85, 0x1F050345},
{ 0x1F86, 0x1F060345},
{ 0x1F87, 0x1F070345},
{ 0x1F0A, 0x1F080300},
{ 0x1F0C, 0x1F080301},
{ 0x1F0E, 0x1F080342},
{ 0x1F88, 0x1F080345},
{ 0x1F0B, 0x1F090300},
{ 0x1F0D, 0x1F090301},
{ 0x1F0F, 0x1F090342},
{ 0x1F89, 0x1F090345},
{ 0x1F8A, 0x1F0A0345},
{ 0x1F8B, 0x1F0B0345},
{ 0x1F8C, 0x1F0C0345},
{ 0x1F8D, 0x1F0D0345},
{ 0x1F8E, 0x1F0E0345},
{ 0x1F8F, 0x1F0F0345},
{ 0x1F12, 0x1F100300},
{ 0x1F14, 0x1F100301},
{ 0x1F13, 0x1F110300},
{ 0x1F15, 0x1F110301},
{ 0x1F1A, 0x1F180300},
{ 0x1F1C, 0x1F180301},
{ 0x1F1B, 0x1F190300},
{ 0x1F1D, 0x1F190301},
{ 0x1F22, 0x1F200300},
{ 0x1F24, 0x1F200301},
{ 0x1F26, 0x1F200342},
{ 0x1F90, 0x1F200345},
{ 0x1F23, 0x1F210300},
{ 0x1F25, 0x1F210301},
{ 0x1F27, 0x1F210342},
{ 0x1F91, 0x1F210345},
{ 0x1F92, 0x1F220345},
{ 0x1F93, 0x1F230345},
{ 0x1F94, 0x1F240345},
{ 0x1F95, 0x1F250345},
{ 0x1F96, 0x1F260345},
{ 0x1F97, 0x1F270345},
{ 0x1F2A, 0x1F280300},
{ 0x1F2C, 0x1F280301},
{ 0x1F2E, 0x1F280342},
{ 0x1F98, 0x1F280345},
{ 0x1F2B, 0x1F290300},
{ 0x1F2D, 0x1F290301},
{ 0x1F2F, 0x1F290342},
{ 0x1F99, 0x1F290345},
{ 0x1F9A, 0x1F2A0345},
{ 0x1F9B, 0x1F2B0345},
{ 0x1F9C, 0x1F2C0345},
{ 0x1F9D, 0x1F2D0345},
{ 0x1F9E, 0x1F2E0345},
{ 0x1F9F, 0x1F2F0345},
{ 0x1F32, 0x1F300300},
{ 0x1F34, 0x1F300301},
{ 0x1F36, 0x1F300342},
{ 0x1F33, 0x1F310300},
{ 0x1F35, 0x1F310301},
{ 0x1F37, 0x1F310342},
{ 0x1F3A, 0x1F380300},
{ 0x1F3C, 0x1F380301},
{ 0x1F3E, 0x1F380342},
{ 0x1F3B, 0x1F390300},
{ 0x1F3D, 0x1F390301},
{ 0x1F3F, 0x1F390342},
{ 0x1F42, 0x1F400300},
{ 0x1F44, 0x1F400301},
{ 0x1F43, 0x1F410300},
{ 0x1F45, 0x1F410301},
{ 0x1F4A, 0x1F480300},
{ 0x1F4C, 0x1F480301},
{ 0x1F4B, 0x1F490300},
{ 0x1F4D, 0x1F490301},
{ 0x1F52, 0x1F500300},
{ 0x1F54, 0x1F500301},
{ 0x1F56, 0x1F500342},
{ 0x1F53, 0x1F510300},
{ 0x1F55, 0x1F510301},
{ 0x1F57, 0x1F510342},
{ 0x1F5B, 0x1F590300},
{ 0x1F5D, 0x1F590301},
{ 0x1F5F, 0x1F590342},
{ 0x1F62, 0x1F600300},
{ 0x1F64, 0x1F600301},
{ 0x1F66, 0x1F600342},
{ 0x1FA0, 0x1F600345},
{ 0x1F63, 0x1F610300},
{ 0x1F65, 0x1F610301},
{ 0x1F67, 0x1F610342},
{ 0x1FA1, 0x1F610345},
{ 0x1FA2, 0x1F620345},
{ 0x1FA3, 0x1F630345},
{ 0x1FA4, 0x1F640345},
{ 0x1FA5, 0x1F650345},
{ 0x1FA6, 0x1F660345},
{ 0x1FA7, 0x1F670345},
{ 0x1F6A, 0x1F680300},
{ 0x1F6C, 0x1F680301},
{ 0x1F6E, 0x1F680342},
{ 0x1FA8, 0x1F680345},
{ 0x1F6B, 0x1F690300},
{ 0x1F6D, 0x1F690301},
{ 0x1F6F, 0x1F690342},
{ 0x1FA9, 0x1F690345},
{ 0x1FAA, 0x1F6A0345},
{ 0x1FAB, 0x1F6B0345},
{ 0x1FAC, 0x1F6C0345},
{ 0x1FAD, 0x1F6D0345},
{ 0x1FAE, 0x1F6E0345},
{ 0x1FAF, 0x1F6F0345},
{ 0x1FB2, 0x1F700345},
{ 0x1FC2, 0x1F740345},
{ 0x1FF2, 0x1F7C0345},
{ 0x1FB7, 0x1FB60345},
{ 0x1FCD, 0x1FBF0300},
{ 0x1FCE, 0x1FBF0301},
{ 0x1FCF, 0x1FBF0342},
{ 0x1FC7, 0x1FC60345},
{ 0x1FF7, 0x1FF60345},
{ 0x1FDD, 0x1FFE0300},
{ 0x1FDE, 0x1FFE0301},
{ 0x1FDF, 0x1FFE0342},
{ 0x219A, 0x21900338},
{ 0x219B, 0x21920338},
{ 0x21AE, 0x21940338},
{ 0x21CD, 0x21D00338},
{ 0x21CF, 0x21D20338},
{ 0x21CE, 0x21D40338},
{ 0x2204, 0x22030338},
{ 0x2209, 0x22080338},
{ 0x220C, 0x220B0338},
{ 0x2224, 0x22230338},
{ 0x2226, 0x22250338},
{ 0x2241, 0x223C0338},
{ 0x2244, 0x22430338},
{ 0x2247, 0x22450338},
{ 0x2249, 0x22480338},
{ 0x226D, 0x224D0338},
{ 0x2262, 0x22610338},
{ 0x2270, 0x22640338},
{ 0x2271, 0x22650338},
{ 0x2274, 0x22720338},
{ 0x2275, 0x22730338},
{ 0x2278, 0x22760338},
{ 0x2279, 0x22770338},
{ 0x2280, 0x227A0338},
{ 0x2281, 0x227B0338},
{ 0x22E0, 0x227C0338},
{ 0x22E1, 0x227D0338},
{ 0x2284, 0x22820338},
{ 0x2285, 0x22830338},
{ 0x2288, 0x22860338},
{ 0x2289, 0x22870338},
{ 0x22E2, 0x22910338},
{ 0x22E3, 0x22920338},
{ 0x22AC, 0x22A20338},
{ 0x22AD, 0x22A80338},
{ 0x22AE, 0x22A90338},
{ 0x22AF, 0x22AB0338},
{ 0x22EA, 0x22B20338},
{ 0x22EB, 0x22B30338},
{ 0x22EC, 0x22B40338},
{ 0x22ED, 0x22B50338},
{ 0x2ADC, 0x2ADD0338},
{ 0x3094, 0x30463099},
{ 0x304C, 0x304B3099},
{ 0x304E, 0x304D3099},
{ 0x3050, 0x304F3099},
{ 0x3052, 0x30513099},
{ 0x3054, 0x30533099},
{ 0x3056, 0x30553099},
{ 0x3058, 0x30573099},
{ 0x305A, 0x30593099},
{ 0x305C, 0x305B3099},
{ 0x305E, 0x305D3099},
{ 0x3060, 0x305F3099},
{ 0x3062, 0x30613099},
{ 0x3065, 0x30643099},
{ 0x3067, 0x30663099},
{ 0x3069, 0x30683099},
{ 0x3070, 0x306F3099},
{ 0x3071, 0x306F309A},
{ 0x3073, 0x30723099},
{ 0x3074, 0x3072309A},
{ 0x3076, 0x30753099},
{ 0x3077, 0x3075309A},
{ 0x3079, 0x30783099},
{ 0x307A, 0x3078309A},
{ 0x307C, 0x307B3099},
{ 0x307D, 0x307B309A},
{ 0x309E, 0x309D3099},
{ 0x30F4, 0x30A63099},
{ 0x30AC, 0x30AB3099},
{ 0x30AE, 0x30AD3099},
{ 0x30B0, 0x30AF3099},
{ 0x30B2, 0x30B13099},
{ 0x30B4, 0x30B33099},
{ 0x30B6, 0x30B53099},
{ 0x30B8, 0x30B73099},
{ 0x30BA, 0x30B93099},
{ 0x30BC, 0x30BB3099},
{ 0x30BE, 0x30BD3099},
{ 0x30C0, 0x30BF3099},
{ 0x30C2, 0x30C13099},
{ 0x30C5, 0x30C43099},
{ 0x30C7, 0x30C63099},
{ 0x30C9, 0x30C83099},
{ 0x30D0, 0x30CF3099},
{ 0x30D1, 0x30CF309A},
{ 0x30D3, 0x30D23099},
{ 0x30D4, 0x30D2309A},
{ 0x30D6, 0x30D53099},
{ 0x30D7, 0x30D5309A},
{ 0x30D9, 0x30D83099},
{ 0x30DA, 0x30D8309A},
{ 0x30DC, 0x30DB3099},
{ 0x30DD, 0x30DB309A},
{ 0x30F7, 0x30EF3099},
{ 0x30F8, 0x30F03099},
{ 0x30F9, 0x30F13099},
{ 0x30FA, 0x30F23099},
{ 0x30FE, 0x30FD3099},
{ 0xFB2C, 0xFB4905C1},
{ 0xFB2D, 0xFB4905C2},
};

// If size of table changes, new delta values are needed !!
// Here: table size N = 997
static int delta[] = { 499, 249, 125, 62, 31, 16, 8, 4, 2, 1, 0 };


/*      Function Name:  UCS2precompose
 *      Description:    Canonically combine two UCS2 characters, if matching
 *                      pattern is found in table. Uniform binary search
 *                      algorithm from D. Knuth TAOCP Vol.3 p.414. Uses static
 *                      arrays table[] and delta[] (see above).
 *      Arguments:      first	- the first UCS2 character
 *                      second	- the second UCS2 character
 *      Returns:        Canonical composition of first and second or
 *                      -1 if no such composition exists in table.
 */
int UCS2precompose(first, second)
char16 first;
char16 second;
{
  int i = delta[0];
  int j = 0;

  unsigned int needle = (first << 16) | second;

  /* uniform binary search */

  while (delta[j++]) {
    if (needle < table[i].pattern) {
      i -= delta[j];
    } else if (needle > table[i].pattern) {
      i += delta[j];
    } else {
      return table[i].precomposed;
    }
  }
  /* precomposed unicode not found */
  return -1;
}

/* ********************************************************************
 *
 * String functions to deal with 16 bit characters.
 * These functions are used to handle strings with 16 bit encoding.
 *
 **********************************************************************/

/*	Function Name:	str16len
 *	Description: 	Determine the string length of a char16 string
 *			independent of the locale settings.
 *	Arguments:	str16	- A terminated string of char16's
 *	Returns:	Length in char16's
 */
int str16len(str16)
char16 *str16;
{
   int len = 0;

   while (*str16++) len++;
   return len;
}

/*	Function Name:	str16chr
 *	Description: 	Search an 8 bit character in a char16 string.
 *			The upper byte of *ch* is assumed as '0'!
 *	Arguments:	str16	- A terminated string of char16's
 *			ch	- An 8 bit character
 *	Returns:	Position of the leftmost occurance of *ch*
 *			in str16 or NULL.
 */
char16 *str16chr(str16, ch)
char16 *str16;
char ch;
{
   char *p;

   while (*str16)
   {
     p = (char *)str16;
     if ((*p++ == '\0') && (*p == ch)) return(str16);
     str16++;
   }
   return NULL;
}

/*	Function Name:	str16cpy
 *	Description: 	Copy a string of char16's from *src* to *dest*
 *	Arguments:	dest	- Destination string
 *			src	- Source string
 *	Returns:	None
 */
void str16cpy(dest, src)
char16 *dest, *src;
{
   while (*src) *dest++ = *src++;
   *dest = 0;		/* To terminate the string */
}

/*	Function Name:	str16ncpy
 *	Description: 	Copy *n* char16's from *src* to *dest* and
 *			terminate *dest*.
 *	Arguments:	dest	- Destination string
 *			src	- Source string
 *			n	- # of characters to copy
 *	Returns:	None	
 */
void str16ncpy(dest, src, n)
char16 *dest, *src;
size_t n;
{
   while ((n > 0) && *src)
   {
     *dest++ = *src++;
     n--;
   }
   *dest = 0;	/* We always terminate the string here */
}

/*	Function Name:	str16cat
 *	Description: 	Concatenate the string of char16's in *src* with *dest*.
 *	Arguments:	dest	- Destination string
 *			src	- Source string
 *	Returns:	None
 */
void str16cat(dest, src)
char16 *dest, *src;
{
   while (*dest) dest++;	  /* search the end of the string */
   while (*src) *dest++ = *src++; /* copy the other behind */
   *dest = 0;			  /* and terminate the string */
}

/*	Function Name:	mbCharLen
 *	Description: 	Determine the length in byte of an UTF8 coded
 *			character.
 *	Arguments:	str	- Pointer into an UTF8 coded string
 *	Returns:	Number of byte of the next character in the string
 *			or 0 in case of an error.
 */
int mbCharLen(str)
char *str;
{
   unsigned char c = (unsigned char)*str;
   unsigned char mask = 0x80;
   int count = 0;

   if (c)
   {
     while (c & mask)
     {
        count++;
        mask >>= 1;
     }

     switch (count)
     {
	case 0:	 return 1;	/* Character is one byte long */
	case 1:  return 0;	/* No multibyte string or wrong position */
	default: return count;
     }
   }
   return 0;	/* End of string reached */
}

/*	Function Name:	mbStrLen
 *	Description: 	Determine the string length of an UTF8 coded string
 *			in characters (not in byte!).
 *	Arguments:	str	- The UTF8 coded string
 *	Returns:	The length in characters, illegal coded bytes
 *			are counted as one character per byte.
 *			See UTF8toUCS2() for the reason!
 */
int mbStrLen(str)
char *str;
{
   char *p = str;
   int clen, len = 0;

   while ((clen = mbCharLen(p)) > 0)
   {
      len++;
      p += clen;
   }
   return len;
}

/*	Function Name:	UTF8toUCS2
 *	Description: 	Conversion of an UTF8 coded string into UCS2/UNICODE.
 *			If the encoding of the character is not representable
 *			in two bytes, the tilde sign ~ is written into the
 *			result string at this position.
 *			For an illegal UTF8 code an asterix * is stored in
 *			the result string.
 *	Arguments:	str	- The UTF8 coded string
 *	Returns:	The UCS2 coded result string. The allocated memory
 *			for this string has to be freed by the caller!
 *			The result string is stored independent of the
 *			architecture in the high byte/low byte order and is
 *			compatible to the XChar2b format! Type casting is valid.
 *			char16 is used to increase the performance.
 */
char16 *UTF8toUCS2(str)
char *str;
{
   char16 *str16, *p16, testINTEL = 0, c16;
   int    clen, cInString;
   char   *p;

  /* In the first step we try to determine the string
   * length in characters.
   */
   cInString = mbStrLen(str);
   cInString++;		/* For the terminating null */

   /* Now we need memory for our conversion result */

   str16 = (char16 *)malloc(cInString * sizeof(char16));
   if (str16)
   {
     /* Start the conversion: Determine the number of bytes
      * for the next character, decode it and store the
      * result in our result string
      */
      p   = str;
      p16 = str16;
      while ((clen = mbCharLen(p)) > 0)
      {
	 switch (clen)
	 {
	    case 1: *p16 = (char16)*p;
		    break;

	    case 2: c16 = (p[1] & 0x3f) + ((p[0] & 0x1f) << 6);
		    *p16 = ((c16 > 0x7f) && ((p[1] & 0xC0) == 0x80)) ? c16 : '*';
		    break;

	    case 3: c16 = (p[2] & 0x3f) + ((p[1] & 0x3f) << 6) + ((p[0] & 0xf) << 12);
		    *p16 = ((c16 > 0x7ff) && ((p[1] & 0xC0) == 0x80) && ((p[2] & 0xC0) == 0x80)) ? c16 : '*';
		    break;

	    default: *p16 = '~';	/* character code is greater than 0xffff */
	 }
	 p16++;				/* Jump to the next character */
	 p += clen;
      }
      *p16 = 0;				/* String termination */

      /* Swap the bytes, if we are on a machine with an INTEL architecture */

      if (*((char *)&testINTEL))
      {
	 char *src, *dest, c;

	 src = dest = (char *)str16;
	 src++;
	 while (*src || *dest)
	 {
	    c = *dest; *dest = *src; *src = c;
	    src += 2; dest += 2;
	 }
      }
      return str16;
   }
   return NULL;
}

/*      Function Name:  UCS2toUTF8
 *      Description:    Conversion of an UCS2 coded string into UTF8.
 *      Arguments:      str16     - The UCS2 coded string
 *      Returns:        The UTF8 coded result string. The allocated memory
 *                      for this string has to be freed by the caller!
 */
char *UCS2toUTF8(str16)
char16 *str16;
{
    char *str8, *p8, *p;
    char16 *p16;
    int len16 = str16len(str16);

    // worst case: 3 bytes of UTF8 per UCS2 char + terminal 0

    str8 = malloc((len16 * 3 * sizeof(char)) + 1);

    p8  = str8;   // reset pointers
    p16 = str16;

    while(*p16 > 0) {
        p = (char*)p16;
        if(*p16 < 0x0080) {
            *p8 = p[0];
            p8++;
        }
        else if(*p16 < 0x0800) {
            *p8 = 0xc0 | ((p[1] & 0x7) << 2) | ((p[0] & 0xc0) >> 6);
            p8++;
            *p8 = 0x80 | (p[0] & 0x3f);
            p8++;
        }
        else {
            *p8 = 0xe0 | ((p[1] & 0xf0) >> 4);
            p8++;
            *p8 = 0x80 | ((p[1] & 0xf) << 2) | ((p[0] & 0xc0) >> 6);
            p8++;
            *p8 = 0x80 | (p[0] & 0x3f);
            p8++; 
        }
        p16++;
    }
    *p8 = 0;  // terminate UTF8 string
    return str8;
}