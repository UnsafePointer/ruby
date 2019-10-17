#include "BIOS.hpp"
#include "Output.hpp"
#include "Helpers.hpp"
#include "Output.hpp"
#include <sstream>

const uint32_t BIOS_A_FUNCTIONS_STEP = 0xA0;
const uint32_t BIOS_B_FUNCTIONS_STEP = 0xB0;
const uint32_t BIOS_C_FUNCTIONS_STEP = 0xC0;

using namespace std;

BIOS::BIOS(LogLevel logLevel) : data(), logger(logLevel) {

}

BIOS::~BIOS() {

}

void BIOS::loadBin(const string& path) {
    readBinary(path, data);
}

std::string BIOS::formatBIOSFunction(std::string function, uint argc, std::array<uint32_t, 4> subroutineArguments) {
    if (argc > 4) {
        logger.logError(format("BIOS formatting incorrect function with argc: %d", argc));
    }

    stringstream ss;
    ss << function;
    if (argc == 0) {
        return ss.str();
    }
    ss << " args: ";
    for (uint i = 0; i < argc; i++) {
        ss << hex << subroutineArguments[i];
        if (i != (argc - 1)) {
            ss << ", ";
        }
    }
    return ss.str();
}

/*
The following three jump tables are taken from no$ documentation:
http://problemkaputt.de/psx-spx.htm#biosfunctionsummary
*/

optional<string> BIOS::checkAFunctions(uint32_t r9, array<uint32_t, 4> subroutineArguments) {
    switch (r9) {
        case 0x00: {
            return { formatBIOSFunction("FileOpen(filename,accessmode)", 2, subroutineArguments) };
        }
        case 0x01: {
            return { formatBIOSFunction("FileSeek(fd,offset,seektype)", 3, subroutineArguments) };
        }
        case 0x02: {
            return { formatBIOSFunction("FileRead(fd,dst,length)", 3, subroutineArguments) };
        }
        case 0x03: {
            return { formatBIOSFunction("FileWrite(fd,src,length)", 3, subroutineArguments) };
        }
        case 0x04: {
            return { formatBIOSFunction("FileClose(fd)", 1, subroutineArguments) };
        }
        case 0x05: {
            return { formatBIOSFunction("FileIoctl(fd,cmd,arg)", 3, subroutineArguments) };
        }
        case 0x06: {
            return { formatBIOSFunction("exit(exitcode)", 1, subroutineArguments) };
        }
        case 0x07: {
            return { formatBIOSFunction("FileGetDeviceFlag(fd)", 1, subroutineArguments) };
        }
        case 0x08: {
            return { formatBIOSFunction("FileGetc(fd)", 1, subroutineArguments) };
        }
        case 0x09: {
            return { formatBIOSFunction("FilePutc(char,fd)", 2, subroutineArguments) };
        }
        case 0x0A: {
            return { formatBIOSFunction("todigit(char)", 1 , subroutineArguments) };
        }
        case 0x0B: {
            return { formatBIOSFunction("atof(src)", 1, subroutineArguments) };
        }
        case 0x0C: {
            return { formatBIOSFunction("strtoul(src,src_end,base)", 3, subroutineArguments) };
        }
        case 0x0D: {
            return { formatBIOSFunction("strtol(src,src_end,base)", 3, subroutineArguments) };
        }
        case 0x0E: {
            return { formatBIOSFunction("abs(val)", 1, subroutineArguments) };
        }
        case 0x0F: {
            return { formatBIOSFunction("labs(val)", 1, subroutineArguments) };
        }
        case 0x10: {
            return { formatBIOSFunction("atoi(src)", 1, subroutineArguments) };
        }
        case 0x11: {
            return { formatBIOSFunction("atol(src)", 1, subroutineArguments) };
        }
        case 0x12: {
            return { formatBIOSFunction("atob(src,num_dst)", 2, subroutineArguments) };
        }
        case 0x13: {
            return { formatBIOSFunction("SaveState(buf)", 1, subroutineArguments) };
        }
        case 0x14: {
            return { formatBIOSFunction("RestoreState(buf,param)", 2, subroutineArguments) };
        }
        case 0x15: {
            return { formatBIOSFunction("strcat(dst,src)", 2, subroutineArguments) };
        }
        case 0x16: {
            return { formatBIOSFunction("strncat(dst,src,maxlen)", 3, subroutineArguments) };
        }
        case 0x17: {
            return { formatBIOSFunction("strcmp(str1,str2)", 2, subroutineArguments) };
        }
        case 0x18: {
            return { formatBIOSFunction("strncmp(str1,str2,maxlen)", 3, subroutineArguments) };
        }
        case 0x19: {
            return { formatBIOSFunction("strcpy(dst,src)", 2, subroutineArguments) };
        }
        case 0x1A: {
            return { formatBIOSFunction("strncpy(dst,src,maxlen)", 3, subroutineArguments) };
        }
        case 0x1B: {
            return { formatBIOSFunction("strlen(src)", 1, subroutineArguments) };
        }
        case 0x1C: {
            return { formatBIOSFunction("index(src,char)", 2, subroutineArguments) };
        }
        case 0x1D: {
            return { formatBIOSFunction("rindex(src,char)", 2, subroutineArguments) };
        }
        case 0x1E: {
            return { formatBIOSFunction("strchr(src,char)", 2, subroutineArguments) };
        }
        case 0x1F: {
            return { formatBIOSFunction("strrchr(src,char)", 2, subroutineArguments) };
        }
        case 0x20: {
            return { formatBIOSFunction("strpbrk(src,list)", 2, subroutineArguments) };
        }
        case 0x21: {
            return { formatBIOSFunction("strspn(src,list)", 2 ,subroutineArguments) };
        }
        case 0x22: {
            return { formatBIOSFunction("strcspn(src,list)", 2, subroutineArguments) };
        }
        case 0x23: {
            return { formatBIOSFunction("strtok(src,list)" , 2, subroutineArguments) };
        }
        case 0x24: {
            return { formatBIOSFunction("strstr(str,substr)", 2, subroutineArguments) };
        }
        case 0x25: {
            return { formatBIOSFunction("toupper(char)", 1, subroutineArguments) };
        }
        case 0x26: {
            return { formatBIOSFunction("tolower(char)", 1, subroutineArguments) };
        }
        case 0x27: {
            return { formatBIOSFunction("bcopy(src,dst,len)", 3, subroutineArguments) };
        }
        case 0x28: {
            return { formatBIOSFunction("bzero(dst,len)", 2, subroutineArguments) };
        }
        case 0x29: {
            return { formatBIOSFunction("bcmp(ptr1,ptr2,len)", 3, subroutineArguments) };
        }
        case 0x2A: {
            return { formatBIOSFunction("memcpy(dst,src,len)", 3, subroutineArguments) };
        }
        case 0x2B: {
            return { formatBIOSFunction("memset(dst,fillbyte,len)", 3, subroutineArguments) };
        }
        case 0x2C: {
            return { formatBIOSFunction("memmove(dst,src,len)", 3, subroutineArguments) };
        }
        case 0x2D: {
            return { formatBIOSFunction("memcmp(src1,src2,len)", 3, subroutineArguments) };
        }
        case 0x2E: {
            return { formatBIOSFunction("memchr(src,scanbyte,len)", 3, subroutineArguments) };
        }
        case 0x2F: {
            return { formatBIOSFunction("rand()", 0, subroutineArguments) };
        }
        case 0x30: {
            return { formatBIOSFunction("srand(seed)", 1, subroutineArguments) };
        }
        case 0x31: {
            return { formatBIOSFunction("qsort(base,nel,width,callback)", 4, subroutineArguments) };
        }
        case 0x32: {
            return { formatBIOSFunction("strtod(src,src_end)", 3, subroutineArguments) };
        }
        case 0x33: {
            return { formatBIOSFunction("malloc(size)", 1, subroutineArguments) };
        }
        case 0x34: {
            return { formatBIOSFunction("free(buf)", 1, subroutineArguments) };
        }
        case 0x35: {
            return { formatBIOSFunction("lsearch(key,base,nel,width,callback)", 4, subroutineArguments) };
        }
        case 0x36: {
            return { formatBIOSFunction("bsearch(key,base,nel,width,callback)", 4, subroutineArguments) };
        }
        case 0x37: {
            return { formatBIOSFunction("calloc(sizx,sizy)", 2, subroutineArguments) };
        }
        case 0x38: {
            return { formatBIOSFunction("realloc(old_buf,new_siz)", 2, subroutineArguments) };
        }
        case 0x39: {
            return { formatBIOSFunction("InitHeap(addr,size)", 2, subroutineArguments) };
        }
        case 0x3A: {
            return { formatBIOSFunction("SystemErrorExit(exitcode)", 1, subroutineArguments) };
        }
        case 0x3B: {
            return { formatBIOSFunction("std_in_getchar()", 0, subroutineArguments) };
        }
        case 0x3C: {
            return { formatBIOSFunction("std_out_putchar(char)", 1, subroutineArguments) };
        }
        case 0x3D: {
            return { formatBIOSFunction("std_in_gets(dst)", 1 ,subroutineArguments) };
        }
        case 0x3E: {
            return { formatBIOSFunction("std_out_puts(src)", 1, subroutineArguments) };
        }
        case 0x3F: {
            return { formatBIOSFunction("printf(txt,param1,param2,etc.)", 4, subroutineArguments) };
        }
        case 0x40: {
            return { formatBIOSFunction("SystemErrorUnresolvedException()", 0, subroutineArguments) };
        }
        case 0x41: {
            return { formatBIOSFunction("LoadExeHeader(filename,headerbuf)", 2, subroutineArguments) };
        }
        case 0x42: {
            return { formatBIOSFunction("LoadExeFile(filename,headerbuf)", 2, subroutineArguments) };
        }
        case 0x43: {
            return { formatBIOSFunction("DoExecute(headerbuf,param1,param2)", 3, subroutineArguments) };
        }
        case 0x44: {
            return { formatBIOSFunction("FlushCache()", 0, subroutineArguments) };
        }
        case 0x45: {
            return { formatBIOSFunction("init_a0_b0_c0_vectors", 0, subroutineArguments) };
        }
        case 0x46: {
            return { formatBIOSFunction("GPU_dw(Xdst,Ydst,Xsiz,Ysiz,src)", 4, subroutineArguments) };
        }
        case 0x47: {
            return { formatBIOSFunction("gpu_send_dma(Xdst,Ydst,Xsiz,Ysiz,src)", 4, subroutineArguments) };
        }
        case 0x48: {
            return { formatBIOSFunction("SendGP1Command(gp1cmd)", 1, subroutineArguments) };
        }
        case 0x49: {
            return { formatBIOSFunction("GPU_cw(gp0cmd)", 1, subroutineArguments) };
        }
        case 0x4A: {
            return { formatBIOSFunction("GPU_cwp(src,num)", 2, subroutineArguments) };
        }
        case 0x4B: {
            return { formatBIOSFunction("send_gpu_linked_list(src)", 1, subroutineArguments) };
        }
        case 0x4C: {
            return { formatBIOSFunction("gpu_abort_dma()", 0, subroutineArguments) };
        }
        case 0x4D: {
            return { formatBIOSFunction("GetGPUStatus()", 0, subroutineArguments) };
        }
        case 0x4E: {
            return { formatBIOSFunction("gpu_sync()", 0, subroutineArguments) };
        }
        case 0x4F: {
            return { formatBIOSFunction("SystemError", 0, subroutineArguments) };
        }
        case 0x50: {
            return { formatBIOSFunction("SystemError", 0, subroutineArguments) };
        }
        case 0x51: {
            return { formatBIOSFunction("LoadAndExecute(filename,stackbase,stackoffset)", 3, subroutineArguments) };
        }
        case 0x52: {
            return { formatBIOSFunction("GetSysSp()", 0, subroutineArguments) };
        }
        case 0x53: {
            return { formatBIOSFunction("SystemError()", 0, subroutineArguments) };
        }
        case 0x54: {
            return { formatBIOSFunction("CdInit()", 0, subroutineArguments) };
        }
        case 0x55: {
            return { formatBIOSFunction("_bu_init()", 0, subroutineArguments) };
        }
        case 0x56: {
            return { formatBIOSFunction("CdRemove()", 0, subroutineArguments) };
        }
        case 0x57: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x58: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x59: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x5A: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x5B: {
            return { formatBIOSFunction("dev_tty_init()", 0, subroutineArguments) };
        }
        case 0x5C: {
            return { formatBIOSFunction("dev_tty_open(fcb,pathname,accessmode)", 3, subroutineArguments) };
        }
        case 0x5D: {
            return { formatBIOSFunction("dev_tty_in_out(fcb,cmd)", 2, subroutineArguments) };
        }
        case 0x5E: {
            return { formatBIOSFunction("dev_tty_ioctl(fcb,cmd,arg)", 3, subroutineArguments) };
        }
        case 0x5F: {
            return { formatBIOSFunction("dev_cd_open(fcb,pathname,accessmode)", 3, subroutineArguments) };
        }
        case 0x60: {
            return { formatBIOSFunction("dev_cd_read(fcb,dst,len)", 3, subroutineArguments) };
        }
        case 0x61: {
            return { formatBIOSFunction("dev_cd_close(fcb)", 1, subroutineArguments) };
        }
        case 0x62: {
            return { formatBIOSFunction("dev_cd_firstfile(fcb,pathname,direntry)", 3, subroutineArguments) };
        }
        case 0x63: {
            return { formatBIOSFunction("dev_cd_nextfile(fcb,direntry)", 2, subroutineArguments) };
        }
        case 0x64: {
            return { formatBIOSFunction("dev_cd_chdir(fcb,path)", 2, subroutineArguments) };
        }
        case 0x65: {
            return { formatBIOSFunction("dev_card_open(fcb,pathname,accessmode)", 3, subroutineArguments) };
        }
        case 0x66: {
            return { formatBIOSFunction("dev_card_read(fcb,dst,len)", 3, subroutineArguments) };
        }
        case 0x67: {
            return { formatBIOSFunction("dev_card_write(fcb,src,len)", 3, subroutineArguments) };
        }
        case 0x68: {
            return { formatBIOSFunction("dev_card_close(fcb)", 1, subroutineArguments) };
        }
        case 0x69: {
            return { formatBIOSFunction("dev_card_firstfile(fcb,pathname,direntry)", 3, subroutineArguments) };
        }
        case 0x6A: {
            return { formatBIOSFunction("dev_card_nextfile(fcb,direntry)", 2, subroutineArguments) };
        }
        case 0x6B: {
            return { formatBIOSFunction("dev_card_erase(fcb,pathname)", 2, subroutineArguments) };
        }
        case 0x6C: {
            return { formatBIOSFunction("dev_card_undelete(fcb,pathname)", 2, subroutineArguments) };
        }
        case 0x6D: {
            return { formatBIOSFunction("dev_card_format(fcb)", 1, subroutineArguments) };
        }
        case 0x6E: {
            return { formatBIOSFunction("dev_card_rename(fcb1,pathname1,fcb2,pathname2)", 4, subroutineArguments) };
        }
        case 0x6F: {
            return { formatBIOSFunction("card_clear_error(fcb)", 1, subroutineArguments) };
        }
        case 0x70: {
            return { formatBIOSFunction("_bu_init()", 0, subroutineArguments) };
        }
        case 0x71: {
            return { formatBIOSFunction("CdInit()", 0, subroutineArguments) };
        }
        case 0x72: {
            return { formatBIOSFunction("CdRemove()", 0, subroutineArguments) };
        }
        case 0x73: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x74: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x75: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x76: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x77: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x78: {
            return { formatBIOSFunction("CdAsyncSeekL(src)", 1, subroutineArguments) };
        }
        case 0x79: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x7A: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x7B: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x7C: {
            return { formatBIOSFunction("CdAsyncGetStatus(dst)", 1, subroutineArguments) };
        }
        case 0x7D: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x7E: {
            return { formatBIOSFunction("CdAsyncReadSector(count,dst,mode)", 3, subroutineArguments) };
        }
        case 0x7F: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x80: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x81: {
            return {formatBIOSFunction( "CdAsyncSetMode(mode)", 1, subroutineArguments) };
        }
        case 0x82: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x83: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x84: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x85: {
            return { formatBIOSFunction("CdStop (?)", 1, subroutineArguments) };
        }
        case 0x86: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x87: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x88: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x89: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x8A: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x8B: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x8C: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x8D: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x8E: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x8F: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x90: {
            return { formatBIOSFunction("CdromIoIrqFunc1()", 0, subroutineArguments) };
        }
        case 0x91: {
            return { formatBIOSFunction("CdromDmaIrqFunc1()", 0, subroutineArguments) };
        }
        case 0x92: {
            return { formatBIOSFunction("CdromIoIrqFunc2()", 0, subroutineArguments) };
        }
        case 0x93: {
            return { formatBIOSFunction("CdromDmaIrqFunc2()", 0, subroutineArguments) };
        }
        case 0x94: {
            return { formatBIOSFunction("CdromGetInt5errCode(dst1,dst2)", 2, subroutineArguments) };
        }
        case 0x95: {
            return { formatBIOSFunction("CdInitSubFunc()", 0, subroutineArguments) };
        }
        case 0x96: {
            return { formatBIOSFunction("AddCDROMDevice()", 0, subroutineArguments) };
        }
        case 0x97: {
            return { formatBIOSFunction("AddMemCardDevice()", 0, subroutineArguments) };
        }
        case 0x98: {
            return { formatBIOSFunction("AddDuartTtyDevice()", 0, subroutineArguments) };
        }
        case 0x99: {
            return { formatBIOSFunction("AddDummyTtyDevice()", 0, subroutineArguments) };
        }
        case 0x9A: {
            return { formatBIOSFunction("AddMessageWindowDevice()", 0, subroutineArguments) };
        }
        case 0x9B: {
            return { formatBIOSFunction("AddCdromSimDevice()", 0, subroutineArguments) };
        }
        case 0x9C: {
            return { formatBIOSFunction("SetConf(num_EvCB,num_TCB,stacktop)", 3, subroutineArguments) };
        }
        case 0x9D: {
            return { formatBIOSFunction("GetConf(num_EvCB_dst,num_TCB_dst,stacktop_dst)", 3, subroutineArguments) };
        }
        case 0x9E: {
            return { formatBIOSFunction("SetCdromIrqAutoAbort(type,flag)", 2, subroutineArguments) };
        }
        case 0x9F: {
            return { formatBIOSFunction("SetMemSize(megabytes)", 1, subroutineArguments) };
        }
        case 0xA0: {
            return { formatBIOSFunction("WarmBoot()", 0, subroutineArguments) };
        }
        case 0xA1: {
            return { formatBIOSFunction("SystemErrorBootOrDiskFailure(type,errorcode)", 2, subroutineArguments) };
        }
        case 0xA2: {
            return { formatBIOSFunction("EnqueueCdIntr()", 0, subroutineArguments) };
        }
        case 0xA3: {
            return { formatBIOSFunction("DequeueCdIntr()", 0, subroutineArguments) };
        }
        case 0xA4: {
            return { formatBIOSFunction("CdGetLbn(filename)", 1, subroutineArguments) };
        }
        case 0xA5: {
            return { formatBIOSFunction("CdReadSector(count,sector,buffer)", 3, subroutineArguments) };
        }
        case 0xA6: {
            return { formatBIOSFunction("CdGetStatus()", 0 , subroutineArguments) };
        }
        case 0xA7: {
            return { formatBIOSFunction("bu_callback_okay()", 0, subroutineArguments) };
        }
        case 0xA8: {
            return { formatBIOSFunction("bu_callback_err_write()", 0, subroutineArguments) };
        }
        case 0xA9: {
            return { formatBIOSFunction("bu_callback_err_busy()", 0, subroutineArguments) };
        }
        case 0xAA: {
            return { formatBIOSFunction("bu_callback_err_eject()", 0, subroutineArguments) };
        }
        case 0xAB: {
            return { formatBIOSFunction("_card_info(port)", 1, subroutineArguments) };
        }
        case 0xAC: {
            return { formatBIOSFunction("_card_async_load_directory(port)", 1, subroutineArguments) };
        }
        case 0xAD: {
            return { formatBIOSFunction("set_card_auto_format(flag)", 1, subroutineArguments) };
        }
        case 0xAE: {
            return { formatBIOSFunction("bu_callback_err_prev_write()", 0, subroutineArguments) };
        }
        case 0xAF: {
            return { formatBIOSFunction("card_write_test(port)", 1, subroutineArguments) };
        }
        case 0xB0: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0xB1: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0xB2: {
            return { formatBIOSFunction("ioabort_raw(param)", 1, subroutineArguments) };
        }
        case 0xB3: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0xB4: {
            return { formatBIOSFunction("GetSystemInfo(index)", 1, subroutineArguments) };
        }
        case 0xB5: {
        }
        case 0xB6: {
        }
        case 0xB7: {
        }
        case 0xB8: {
        }
        case 0xB9: {
        }
        case 0xBA: {
        }
        case 0xBB: {
        }
        case 0xBC: {
        }
        case 0xBD: {
        }
        case 0xBE: {
        }
        case 0xBF: {
            return { formatBIOSFunction("N/A", 4, subroutineArguments) };
        }
        default: {
            stringstream ss;
            ss << "Unknown A function with r9: " << std::hex << r9;
            return { formatBIOSFunction(ss.str(), 4, subroutineArguments) };
        }
    }
}

optional<string> BIOS::checkBFunctions(uint32_t r9, array<uint32_t, 4> subroutineArguments) {
    switch (r9) {
        case 0x00: {
            return { formatBIOSFunction("alloc_kernel_memory(size)", 1, subroutineArguments) };
        }
        case 0x01: {
            return { formatBIOSFunction("free_kernel_memory(buf)", 1, subroutineArguments) };
        }
        case 0x02: {
            return { formatBIOSFunction("init_timer(t,reload,flags)", 3, subroutineArguments) };
        }
        case 0x03: {
            return { formatBIOSFunction("get_timer(t)", 1, subroutineArguments) };
        }
        case 0x04: {
            return { formatBIOSFunction("enable_timer_irq(t)", 1, subroutineArguments) };
        }
        case 0x05: {
            return { formatBIOSFunction("disable_timer_irq(t)", 1, subroutineArguments) };
        }
        case 0x06: {
            return { formatBIOSFunction("restart_timer(t)", 1, subroutineArguments) };
        }
        case 0x07: {
            return { formatBIOSFunction("DeliverEvent(class, spec)", 2, subroutineArguments) };
        }
        case 0x08: {
            return { formatBIOSFunction("OpenEvent(class,spec,mode,func)", 4, subroutineArguments) };
        }
        case 0x09: {
            return { formatBIOSFunction("CloseEvent(event)", 1, subroutineArguments) };
        }
        case 0x0A: {
            return { formatBIOSFunction("WaitEvent(event)", 1, subroutineArguments) };
        }
        case 0x0B: {
            return { formatBIOSFunction("TestEvent(event)", 1, subroutineArguments) };
        }
        case 0x0C: {
            return { formatBIOSFunction("EnableEvent(event)", 1, subroutineArguments) };
        }
        case 0x0D: {
            return { formatBIOSFunction("DisableEvent(event)", 1, subroutineArguments) };
        }
        case 0x0E: {
            return { formatBIOSFunction("OpenThread(reg_PC,reg_SP_FP,reg_GP)", 3, subroutineArguments) };
        }
        case 0x0F: {
            return { formatBIOSFunction("CloseThread(handle)", 1, subroutineArguments) };
        }
        case 0x10: {
            return { formatBIOSFunction("ChangeThread(handle)", 1, subroutineArguments) };
        }
        case 0x11: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x12: {
            return { formatBIOSFunction("InitPad(buf1,siz1,buf2,siz2)", 4, subroutineArguments) };
        }
        case 0x13: {
            return { formatBIOSFunction("StartPad()", 0, subroutineArguments) };
        }
        case 0x14: {
            return { formatBIOSFunction("StopPad()", 0, subroutineArguments) };
        }
        case 0x15: {
            return { formatBIOSFunction("OutdatedPadInitAndStart(type,button_dest,unused,unused)", 4, subroutineArguments) };
        }
        case 0x16: {
            return { formatBIOSFunction("OutdatedPadGetButtons()", 0, subroutineArguments) };
        }
        case 0x17: {
            return { formatBIOSFunction("ReturnFromException()", 0, subroutineArguments) };
        }
        case 0x18: {
            return { formatBIOSFunction("SetDefaultExitFromException()", 0 ,subroutineArguments) };
        }
        case 0x19: {
            return { formatBIOSFunction("SetCustomExitFromException(addr)", 1, subroutineArguments) };
        }
        case 0x1A: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x1B: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x1C: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x1D: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x1E: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x1F: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x20: {
            return { formatBIOSFunction("UnDeliverEvent(class,spec)", 2, subroutineArguments) };
        }
        case 0x21: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x22: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x23: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x24: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x25: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x26: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x27: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x28: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x29: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x2A: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x2B: {
            return { formatBIOSFunction("return 0", 0, subroutineArguments) };
        }
        case 0x2C: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x2D: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x2E: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x2F: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x30: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x31: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        case 0x32: {
            return { formatBIOSFunction("FileOpen(filename,accessmode)", 2, subroutineArguments) };
        }
        case 0x33: {
            return { formatBIOSFunction("FileSeek(fd,offset,seektype)", 3, subroutineArguments) };
        }
        case 0x34: {
            return { formatBIOSFunction("FileRead(fd,dst,length)", 3, subroutineArguments) };
        }
        case 0x35: {
            return { formatBIOSFunction("FileWrite(fd,src,length)", 3, subroutineArguments) };
        }
        case 0x36: {
            return { formatBIOSFunction("FileClose(fd)", 1, subroutineArguments) };
        }
        case 0x37: {
            return { formatBIOSFunction("FileIoctl(fd,cmd,arg)", 3, subroutineArguments) };
        }
        case 0x38: {
            return { formatBIOSFunction("exit(exitcode)", 1, subroutineArguments) };
        }
        case 0x39: {
            return { formatBIOSFunction("FileGetDeviceFlag(fd)", 1, subroutineArguments) };
        }
        case 0x3A: {
            return { formatBIOSFunction("FileGetc(fd)", 1, subroutineArguments) };
        }
        case 0x3B: {
            return { formatBIOSFunction("FilePutc(char,fd)", 2, subroutineArguments) };
        }
        case 0x3C: {
            return { formatBIOSFunction("std_in_getchar()", 0, subroutineArguments) };
        }
        case 0x3D: {
            return { formatBIOSFunction("std_out_putchar(char)", 1, subroutineArguments) };
        }
        case 0x3E: {
            return { formatBIOSFunction("std_in_gets(dst)", 1, subroutineArguments) };
        }
        case 0x3F: {
            return { formatBIOSFunction("std_out_puts(src)", 1, subroutineArguments) };
        }
        case 0x40: {
            return { formatBIOSFunction("chdir(name)", 1, subroutineArguments) };
        }
        case 0x41: {
            return { formatBIOSFunction("FormatDevice(devicename)", 1, subroutineArguments) };
        }
        case 0x42: {
            return { formatBIOSFunction("firstfile(filename,direntry)", 2, subroutineArguments) };
        }
        case 0x43: {
            return { formatBIOSFunction("nextfile(direntry)", 1, subroutineArguments) };
        }
        case 0x44: {
            return { formatBIOSFunction("FileRename(old_filename,new_filename)", 2, subroutineArguments) };
        }
        case 0x45: {
            return { formatBIOSFunction("FileDelete(filename)", 1, subroutineArguments) };
        }
        case 0x46: {
            return { formatBIOSFunction("FileUndelete(filename)", 1, subroutineArguments) };
        }
        case 0x47: {
            return { formatBIOSFunction("AddDevice(device_info)", 1, subroutineArguments) };
        }
        case 0x48: {
            return { formatBIOSFunction("RemoveDevice(device_name_lowercase)", 1, subroutineArguments) };
        }
        case 0x49: {
            return { formatBIOSFunction("PrintInstalledDevices()", 0, subroutineArguments) };
        }
        case 0x4A: {
            return { formatBIOSFunction("InitCard(pad_enable)", 1, subroutineArguments) };
        }
        case 0x4B: {
            return { formatBIOSFunction("StartCard()", 0, subroutineArguments) };
        }
        case 0x4C: {
            return { formatBIOSFunction("StopCard()", 0 ,subroutineArguments) };
        }
        case 0x4D: {
            return { formatBIOSFunction("_card_info_subfunc(port)", 1, subroutineArguments) };
        }
        case 0x4E: {
            return { formatBIOSFunction("write_card_sector(port,sector,src)", 3, subroutineArguments) };
        }
        case 0x4F: {
            return { formatBIOSFunction("read_card_sector(port,sector,dst)", 3, subroutineArguments) };
        }
        case 0x50: {
            return { formatBIOSFunction("allow_new_card()", 0, subroutineArguments) };
        }
        case 0x51: {
            return { formatBIOSFunction("Krom2RawAdd(shiftjis_code)", 1, subroutineArguments) };
        }
        case 0x52: {
            return { formatBIOSFunction("SystemError", 0 ,subroutineArguments) };
        }
        case 0x53: {
            return { formatBIOSFunction("Krom2Offset(shiftjis_code)", 1, subroutineArguments) };
        }
        case 0x54: {
            return { formatBIOSFunction("GetLastError()", 0, subroutineArguments) };
        }
        case 0x55: {
            return { formatBIOSFunction("GetLastFileError(fd)", 1, subroutineArguments) };
        }
        case 0x56: {
            return { formatBIOSFunction("GetC0Table()", 0, subroutineArguments) };
        }
        case 0x57: {
            return { formatBIOSFunction("GetB0Table()", 0, subroutineArguments) };
        }
        case 0x58: {
            return { formatBIOSFunction("get_bu_callback_port()", 0, subroutineArguments) };
        }
        case 0x59: {
            return { formatBIOSFunction("testdevice(devicename)", 1, subroutineArguments) };
        }
        case 0x5A: {
            return { formatBIOSFunction("SystemError", 0, subroutineArguments) };
        }
        case 0x5B: {
            return { formatBIOSFunction("ChangeClearPad(int)", 1, subroutineArguments) };
        }
        case 0x5C: {
            return { formatBIOSFunction("get_card_status(slot)", 1, subroutineArguments) };
        }
        case 0x5D: {
            return { formatBIOSFunction("wait_card_status(slot)", 1, subroutineArguments) };
        }
        case 0x5E: {
            return { formatBIOSFunction("jump_to_00000000h", 4, subroutineArguments) };
        }
        default: {
            stringstream ss;
            ss << "Unknown B function with r9: " << std::hex << r9;
            return { formatBIOSFunction(ss.str(), 4, subroutineArguments) };
        }
    }
}

optional<string> BIOS::checkCFunctions(uint32_t r9, array<uint32_t, 4> subroutineArguments) {
    switch (r9) {
        case 0x00: {
            return { formatBIOSFunction("EnqueueTimerAndVblankIrqs(priority)", 1, subroutineArguments) };
        }
        case 0x01: {
            return { formatBIOSFunction("EnqueueSyscallHandler(priority)", 1, subroutineArguments) };
        }
        case 0x02: {
            return { formatBIOSFunction("SysEnqIntRP(priority,struc)", 2, subroutineArguments) };
        }
        case 0x03: {
            return { formatBIOSFunction("SysDeqIntRP(priority,struc)", 2, subroutineArguments) };
        }
        case 0x04: {
            return { formatBIOSFunction("get_free_EvCB_slot()", 0, subroutineArguments) };
        }
        case 0x05: {
            return { formatBIOSFunction("get_free_TCB_slot()", 0, subroutineArguments) };
        }
        case 0x06: {
            return { formatBIOSFunction("ExceptionHandler()", 0, subroutineArguments) };
        }
        case 0x07: {
            return { formatBIOSFunction("InstallExceptionHandlers()", 0, subroutineArguments) };
        }
        case 0x08: {
            return { formatBIOSFunction("SysInitMemory(addr,size)", 2, subroutineArguments) };
        }
        case 0x09: {
            return { formatBIOSFunction("SysInitKernelVariables()", 0, subroutineArguments) };
        }
        case 0x0A: {
            return { formatBIOSFunction("ChangeClearRCnt(t,flag)", 2, subroutineArguments) };
        }
        case 0x0B: {
            return { formatBIOSFunction("SystemError", 0, subroutineArguments) };
        }
        case 0x0C: {
            return { formatBIOSFunction("InitDefInt(priority)", 1, subroutineArguments) };
        }
        case 0x0D: {
            return { formatBIOSFunction("SetIrqAutoAck(irq,flag)", 2, subroutineArguments) };
        }
        case 0x0E: {
            return { formatBIOSFunction("dev_sio_init()", 4, subroutineArguments) };
        }
        case 0x0F: {
            return { formatBIOSFunction("dev_sio_open()", 4, subroutineArguments) };
        }
        case 0x10: {
            return { formatBIOSFunction("dev_sio_in_out()", 4, subroutineArguments) };
        }
        case 0x11: {
            return { formatBIOSFunction("dev_sio_ioctl()", 0, subroutineArguments) };
        }
        case 0x12: {
            return { formatBIOSFunction("InstallDevices(ttyflag)", 1, subroutineArguments) };
        }
        case 0x13: {
            return { formatBIOSFunction("FlushStdInOutPut()", 0, subroutineArguments) };
        }
        case 0x14: {
            return { formatBIOSFunction("SystemError", 0, subroutineArguments) };
        }
        case 0x15: {
            return { formatBIOSFunction("tty_cdevinput(circ,char)", 2, subroutineArguments) };
        }
        case 0x16: {
            return { formatBIOSFunction("tty_cdevscan()", 0, subroutineArguments) };
        }
        case 0x17: {
            return { formatBIOSFunction("tty_circgetc(circ)", 1, subroutineArguments) };
        }
        case 0x18: {
            return { formatBIOSFunction("tty_circputc(char,circ)", 2, subroutineArguments) };
        }
        case 0x19: {
            return { formatBIOSFunction("ioabort(txt1,txt2)", 2, subroutineArguments) };
        }
        case 0x1A: {
            return { formatBIOSFunction("set_card_find_mode(mode)", 1, subroutineArguments) };
        }
        case 0x1B: {
            return { formatBIOSFunction("KernelRedirect(ttyflag)", 1, subroutineArguments) };
        }
        case 0x1C: {
            return { formatBIOSFunction("AdjustA0Table()", 0, subroutineArguments) };
        }
        case 0x1D: {
            return { formatBIOSFunction("get_card_find_mode()", 0, subroutineArguments) };
        }
        default: {
            stringstream ss;
            ss << "Unknown C function with r9: " << std::hex << r9;
            return { formatBIOSFunction(ss.str(), 4, subroutineArguments) };
        }
    }
}

optional<string> BIOS::checkFunctions(uint32_t programCounter, uint32_t r9, array<uint32_t, 4> subroutineArguments) {
    optional<string> result;
    switch (programCounter) {
        case BIOS_A_FUNCTIONS_STEP: {
            result = checkAFunctions(r9, subroutineArguments);
            break;
        }
        case BIOS_B_FUNCTIONS_STEP: {
            result = checkBFunctions(r9, subroutineArguments);
            break;
        }
        case BIOS_C_FUNCTIONS_STEP: {
            result = checkCFunctions(r9, subroutineArguments);
            break;
        }
        default: {
            result = { nullopt };
        }
    }
    if (!result) {
        return result;
    }
    string functionCallLog = (*result);
    bool functionCallLogIsRFE = functionCallLog.find("ReturnFromException()") == 0;
    if (functionCallLogIsRFE) {
        return result;
    }
    logger.logMessage(format("  BIOS: %s", functionCallLog.c_str()));
}
