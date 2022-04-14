#ifndef __DecodeCapiMessages_H__
#define __DecodeCapiMessages_H__

#define		BUFFER_32	((unsigned int)32)
#define		BUFFER_64	((unsigned int)64)
#define		CheckIfByte(a) ASSERT(a<=0xFF);
#define		CheckIfWord(a) ASSERT(a<= 0xFFFF);
char *Decode_Info	(unsigned int Info);
char *Decode_Command(unsigned char Command);
char *Decode_Sub	(unsigned char Sub);

#endif