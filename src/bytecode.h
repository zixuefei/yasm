/* $Id: bytecode.h,v 1.14 2001/07/11 23:16:50 peter Exp $
 * Bytecode utility functions header file
 *
 *  Copyright (C) 2001  Peter Johnson
 *
 *  This file is part of YASM.
 *
 *  YASM is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  YASM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef YASM_BYTECODE_H
#define YASM_BYTECODE_H

typedef struct effaddr_s {
    struct expr_s *disp;	/* address displacement */
    unsigned char len;		/* length of disp (in bytes), 0 if none */

    unsigned char segment;	/* segment override, 0 if none */

    unsigned char modrm;
    unsigned char valid_modrm;	/* 1 if Mod/RM byte currently valid, 0 if not */
    unsigned char need_modrm;	/* 1 if Mod/RM byte needed, 0 if not */

    unsigned char sib;
    unsigned char valid_sib;	/* 1 if SIB byte currently valid, 0 if not */
    unsigned char need_sib;	/* 1 if SIB byte needed, 0 if not */
} effaddr;

typedef struct immval_s {
    struct expr_s *val;

    unsigned char len;		/* length of val (in bytes), 0 if none */
    unsigned char isneg;	/* the value has been explicitly negated */

    unsigned char f_len;	/* final imm length */
    unsigned char f_sign;	/* 1 if final imm should be signed */
} immval;

typedef enum jmprel_opcode_sel_e {
    JR_NONE,
    JR_SHORT,
    JR_NEAR,
    JR_SHORT_FORCED,
    JR_NEAR_FORCED
} jmprel_opcode_sel;

typedef struct targetval_s {
    struct expr_s *val;

    jmprel_opcode_sel op_sel;
} targetval;

typedef struct bytecode_s {
    struct bytecode_s *next;

    enum { BC_INSN, BC_JMPREL, BC_DATA, BC_RESERVE } type;

    union {
	struct {
	    effaddr ea;			/* effective address */

	    immval imm;			/* immediate or relative value */

	    unsigned char opcode[3];	/* opcode */
	    unsigned char opcode_len;

	    unsigned char addrsize;	/* 0 indicates no override */
	    unsigned char opersize;	/* 0 indicates no override */
	    unsigned char lockrep_pre;	/* 0 indicates no prefix */
	} insn;
	struct {
	    struct expr_s *target;	/* target location */

	    struct {
		unsigned char opcode[3];
		unsigned char opcode_len;
		unsigned char valid;	/* does the opcode exist? */
	    } shortop, nearop;

	    /* which opcode are we using? */
	    /* The *FORCED forms are specified in the source as such */
	    jmprel_opcode_sel op_sel;

	    unsigned char addrsize;	/* 0 indicates no override */
	    unsigned char opersize;	/* 0 indicates no override */
	    unsigned char lockrep_pre;	/* 0 indicates no prefix */
	} jmprel;
	struct {
	    unsigned char *data;
	} data;
    } data;

    unsigned long len;	/* total length of entire bytecode */

    /* where it came from */
    char *filename;
    unsigned int lineno;

    /* other assembler state info */
    unsigned long offset;
    unsigned int mode_bits;
} bytecode;

effaddr *ConvertRegToEA(effaddr *ptr, unsigned long reg);
effaddr *ConvertImmToEA(effaddr *ptr, immval *im_ptr, unsigned char im_len);
effaddr *ConvertExprToEA(effaddr *ptr, struct expr_s *expr_ptr);

immval *ConvertIntToImm(immval *ptr, unsigned long int_val);
immval *ConvertExprToImm(immval *ptr, struct expr_s *expr_ptr);

void SetEASegment(effaddr *ptr, unsigned char segment);
void SetEALen(effaddr *ptr, unsigned char len);

void SetInsnOperSizeOverride(bytecode *bc, unsigned char opersize);
void SetInsnAddrSizeOverride(bytecode *bc, unsigned char addrsize);
void SetInsnLockRepPrefix(bytecode *bc, unsigned char prefix);

void SetOpcodeSel(jmprel_opcode_sel *old_sel, jmprel_opcode_sel new_sel);

void BuildBC_Insn(bytecode      *bc,
		  unsigned char  opersize,
		  unsigned char  opcode_len,
		  unsigned char  op0,
		  unsigned char  op1,
		  unsigned char  op2,
		  effaddr       *ea_ptr,
		  unsigned char  spare,
		  immval        *im_ptr,
		  unsigned char  im_len,
		  unsigned char  im_sign);

void BuildBC_JmpRel(bytecode      *bc,
		    targetval     *target,
		    unsigned char  short_valid,
		    unsigned char  short_opcode_len,
		    unsigned char  short_op0,
		    unsigned char  short_op1,
		    unsigned char  short_op2,
		    unsigned char  near_valid,
		    unsigned char  near_opcode_len,
		    unsigned char  near_op0,
		    unsigned char  near_op1,
		    unsigned char  near_op2,
		    unsigned char  addrsize);

unsigned char *ConvertBCInsnToBytes(unsigned char *ptr, bytecode *bc, int *len);

void DebugPrintBC(bytecode *bc);

#endif
