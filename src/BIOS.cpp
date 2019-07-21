#include "BIOS.hpp"
#include "Output.hpp"
#include "Helpers.hpp"

const uint32_t BIOS_A_FUNCTIONS_STEP = 0xA0;
const uint32_t BIOS_B_FUNCTIONS_STEP = 0xB0;
const uint32_t BIOS_C_FUNCTIONS_STEP = 0xC0;

using namespace std;

BIOS::BIOS() : data() {

}

BIOS::~BIOS() {

}

void BIOS::loadBin(const string& path) {
    readBinary(path, data);
}

/*
The following three jump tables are taken from no$ documentation:
http://problemkaputt.de/psx-spx.htm#biosfunctionsummary
*/

optional<string> BIOS::checkAFunctions(uint32_t r9) {
    switch (r9) {
        case 0x00: {
            return { "FileOpen(filename,accessmode)" };
        }
        case 0x01: {
            return { "FileSeek(fd,offset,seektype)" };
        }
        case 0x02: {
            return { "FileRead(fd,dst,length)" };
        }
        case 0x03: {
            return { "FileWrite(fd,src,length)" };
        }
        case 0x04: {
            return { "FileClose(fd)" };
        }
        case 0x05: {
            return { "FileIoctl(fd,cmd,arg)" };
        }
        case 0x06: {
            return { "exit(exitcode)" };
        }
        case 0x07: {
            return { "FileGetDeviceFlag(fd)" };
        }
        case 0x08: {
            return { "FileGetc(fd)" };
        }
        case 0x09: {
            return { "FilePutc(char,fd)" };
        }
        case 0x0A: {
            return { "todigit(char)" };
        }
        case 0x0B: {
            return { "atof(src)     ;Does NOT work - uses (ABSENT) cop1 !!!" };
        }
        case 0x0C: {
            return { "strtoul(src,src_end,base)" };
        }
        case 0x0D: {
            return { "strtol(src,src_end,base)" };
        }
        case 0x0E: {
            return { "abs(val)" };
        }
        case 0x0F: {
            return { "labs(val)" };
        }
        case 0x10: {
            return { "atoi(src)" };
        }
        case 0x11: {
            return { "atol(src)" };
        }
        case 0x12: {
            return { "atob(src,num_dst)" };
        }
        case 0x13: {
            return { "SaveState(buf)" };
        }
        case 0x14: {
            return { "RestoreState(buf,param)" };
        }
        case 0x15: {
            return { "strcat(dst,src)" };
        }
        case 0x16: {
            return { "strncat(dst,src,maxlen)" };
        }
        case 0x17: {
            return { "strcmp(str1,str2)" };
        }
        case 0x18: {
            return { "strncmp(str1,str2,maxlen)" };
        }
        case 0x19: {
            return { "strcpy(dst,src)" };
        }
        case 0x1A: {
            return { "strncpy(dst,src,maxlen)" };
        }
        case 0x1B: {
            return { "strlen(src)" };
        }
        case 0x1C: {
            return { "index(src,char)" };
        }
        case 0x1D: {
            return { "rindex(src,char)" };
        }
        case 0x1E: {
            return { "strchr(src,char)" };
        }
        case 0x1F: {
            return { "strrchr(src,char)" };
        }
        case 0x20: {
            return { "strpbrk(src,list)" };
        }
        case 0x21: {
            return { "strspn(src,list)" };
        }
        case 0x22: {
            return { "strcspn(src,list)" };
        }
        case 0x23: {
            return { "strtok(src,list)" };
        }
        case 0x24: {
            return { "strstr(str,substr)" };
        }
        case 0x25: {
            return { "toupper(char)" };
        }
        case 0x26: {
            return { "tolower(char)" };
        }
        case 0x27: {
            return { "bcopy(src,dst,len)" };
        }
        case 0x28: {
            return { "bzero(dst,len)" };
        }
        case 0x29: {
            return { "bcmp(ptr1,ptr2,len)" };
        }
        case 0x2A: {
            return { "memcpy(dst,src,len)" };
        }
        case 0x2B: {
            return { "memset(dst,fillbyte,len)" };
        }
        case 0x2C: {
            return { "memmove(dst,src,len)" };
        }
        case 0x2D: {
            return { "memcmp(src1,src2,len)" };
        }
        case 0x2E: {
            return { "memchr(src,scanbyte,len)" };
        }
        case 0x2F: {
            return { "rand()" };
        }
        case 0x30: {
            return { "srand(seed)" };
        }
        case 0x31: {
            return { "qsort(base,nel,width,callback)" };
        }
        case 0x32: {
            return { "strtod(src,src_end)" };
        }
        case 0x33: {
            return { "malloc(size)" };
        }
        case 0x34: {
            return { "free(buf)" };
        }
        case 0x35: {
            return { "lsearch(key,base,nel,width,callback)" };
        }
        case 0x36: {
            return { "bsearch(key,base,nel,width,callback)" };
        }
        case 0x37: {
            return { "calloc(sizx,sizy)" };
        }
        case 0x38: {
            return { "realloc(old_buf,new_siz)" };
        }
        case 0x39: {
            return { "InitHeap(addr,size)" };
        }
        case 0x3A: {
            return { "SystemErrorExit(exitcode)" };
        }
        case 0x3B: {
            return { "std_in_getchar()" };
        }
        case 0x3C: {
            return { "std_out_putchar(char)" };
        }
        case 0x3D: {
            return { "std_in_gets(dst)" };
        }
        case 0x3E: {
            return { "std_out_puts(src)" };
        }
        case 0x3F: {
            return { "printf(txt,param1,param2,etc.)" };
        }
        case 0x40: {
            return { "SystemErrorUnresolvedException()" };
        }
        case 0x41: {
            return { "LoadExeHeader(filename,headerbuf)" };
        }
        case 0x42: {
            return { "LoadExeFile(filename,headerbuf)" };
        }
        case 0x43: {
            return { "DoExecute(headerbuf,param1,param2)" };
        }
        case 0x44: {
            return { "FlushCache()" };
        }
        case 0x45: {
            return { "init_a0_b0_c0_vectors" };
        }
        case 0x46: {
            return { "GPU_dw(Xdst,Ydst,Xsiz,Ysiz,src)" };
        }
        case 0x47: {
            return { "gpu_send_dma(Xdst,Ydst,Xsiz,Ysiz,src)" };
        }
        case 0x48: {
            return { "SendGP1Command(gp1cmd)" };
        }
        case 0x49: {
            return { "GPU_cw(gp0cmd)" };
        }
        case 0x4A: {
            return { "GPU_cwp(src,num)" };
        }
        case 0x4B: {
            return { "send_gpu_linked_list(src)" };
        }
        case 0x4C: {
            return { "gpu_abort_dma()" };
        }
        case 0x4D: {
            return { "GetGPUStatus()" };
        }
        case 0x4E: {
            return { "gpu_sync()" };
        }
        case 0x4F: {
            return { "SystemError" };
        }
        case 0x50: {
            return { "SystemError" };
        }
        case 0x51: {
            return { "LoadAndExecute(filename,stackbase,stackoffset)" };
        }
        case 0x52: {
            return { "GetSysSp()" };
        }
        case 0x53: {
            return { "SystemError()" };
        }
        case 0x54: {
            return { "CdInit()" };
        }
        case 0x55: {
            return { "_bu_init()" };
        }
        case 0x56: {
            return { "CdRemove()" };
        }
        case 0x57: {
            return { "return 0" };
        }
        case 0x58: {
            return { "return 0" };
        }
        case 0x59: {
            return { "return 0" };
        }
        case 0x5A: {
            return { "return 0" };
        }
        case 0x5B: {
            return { "dev_tty_init()" };
        }
        case 0x5C: {
            return { "dev_tty_open(fcb,pathname,accessmode)" };
        }
        case 0x5D: {
            return { "dev_tty_in_out(fcb,cmd)" };
        }
        case 0x5E: {
            return { "dev_tty_ioctl(fcb,cmd,arg)" };
        }
        case 0x5F: {
            return { "dev_cd_open(fcb,pathname,accessmode" };
        }
        case 0x60: {
            return { "dev_cd_read(fcb,dst,len)" };
        }
        case 0x61: {
            return { "dev_cd_close(fcb)" };
        }
        case 0x62: {
            return { "dev_cd_firstfile(fcb,pathname,direntry)" };
        }
        case 0x63: {
            return { "dev_cd_nextfile(fcb,direntry)" };
        }
        case 0x64: {
            return { "dev_cd_chdir(fcb,path)" };
        }
        case 0x65: {
            return { "dev_card_open(fcb,pathname,accessmode)" };
        }
        case 0x66: {
            return { "dev_card_read(fcb,dst,len)" };
        }
        case 0x67: {
            return { "dev_card_write(fcb,src,len)" };
        }
        case 0x68: {
            return { "dev_card_close(fcb)" };
        }
        case 0x69: {
            return { "dev_card_firstfile(fcb,pathname,direntry)" };
        }
        case 0x6A: {
            return { "dev_card_nextfile(fcb,direntry)" };
        }
        case 0x6B: {
            return { "dev_card_erase(fcb,pathname)" };
        }
        case 0x6C: {
            return { "dev_card_undelete(fcb,pathname)" };
        }
        case 0x6D: {
            return { "dev_card_format(fcb)" };
        }
        case 0x6E: {
            return { "dev_card_rename(fcb1,pathname1,fcb2,pathname2)" };
        }
        case 0x6F: {
            return { "card_clear_error(fcb)" };
        }
        case 0x70: {
            return { "_bu_init()" };
        }
        case 0x71: {
            return { "CdInit()" };
        }
        case 0x72: {
            return { "CdRemove()" };
        }
        case 0x73: {
            return { "return 0" };
        }
        case 0x74: {
            return { "return 0" };
        }
        case 0x75: {
            return { "return 0" };
        }
        case 0x76: {
            return { "return 0" };
        }
        case 0x77: {
            return { "return 0" };
        }
        case 0x78: {
            return { "CdAsyncSeekL(src)" };
        }
        case 0x79: {
            return { "return 0" };
        }
        case 0x7A: {
            return { "return 0" };
        }
        case 0x7B: {
            return { "return 0" };
        }
        case 0x7C: {
            return { "CdAsyncGetStatus(dst)" };
        }
        case 0x7D: {
            return { "return 0" };
        }
        case 0x7E: {
            return { "CdAsyncReadSector(count,dst,mode)" };
        }
        case 0x7F: {
            return { "return 0" };
        }
        case 0x80: {
            return { "return 0" };
        }
        case 0x81: {
            return { "CdAsyncSetMode(mode)" };
        }
        case 0x82: {
            return { "return 0" };
        }
        case 0x83: {
            return { "return 0" };
        }
        case 0x84: {
            return { "return 0" };
        }
        case 0x85: {
            return { "CdStop (?)" };
        }
        case 0x86: {
            return { "return 0" };
        }
        case 0x87: {
            return { "return 0" };
        }
        case 0x88: {
            return { "return 0" };
        }
        case 0x89: {
            return { "return 0" };
        }
        case 0x8A: {
            return { "return 0" };
        }
        case 0x8B: {
            return { "return 0" };
        }
        case 0x8C: {
            return { "return 0" };
        }
        case 0x8D: {
            return { "return 0" };
        }
        case 0x8E: {
            return { "return 0" };
        }
        case 0x8F: {
            return { "return 0" };
        }
        case 0x90: {
            return { "CdromIoIrqFunc1()" };
        }
        case 0x91: {
            return { "CdromDmaIrqFunc1()" };
        }
        case 0x92: {
            return { "CdromIoIrqFunc2()" };
        }
        case 0x93: {
            return { "CdromDmaIrqFunc2()" };
        }
        case 0x94: {
            return { "CdromGetInt5errCode(dst1,dst2)" };
        }
        case 0x95: {
            return { "CdInitSubFunc()" };
        }
        case 0x96: {
            return { "AddCDROMDevice()" };
        }
        case 0x97: {
            return { "AddMemCardDevice()" };
        }
        case 0x98: {
            return { "AddDuartTtyDevice()" };
        }
        case 0x99: {
            return { "AddDummyTtyDevice()" };
        }
        case 0x9A: {
            return { "AddMessageWindowDevice" };
        }
        case 0x9B: {
            return { "AddCdromSimDevice" };
        }
        case 0x9C: {
            return { "SetConf(num_EvCB,num_TCB,stacktop)" };
        }
        case 0x9D: {
            return { "GetConf(num_EvCB_dst,num_TCB_dst,stacktop_dst)" };
        }
        case 0x9E: {
            return { "SetCdromIrqAutoAbort(type,flag)" };
        }
        case 0x9F: {
            return { "SetMemSize(megabytes)" };
        }
        case 0xA0: {
            return { "WarmBoot()" };
        }
        case 0xA1: {
            return { "SystemErrorBootOrDiskFailure(type,errorcode)" };
        }
        case 0xA2: {
            return { "EnqueueCdIntr()" };
        }
        case 0xA3: {
            return { "DequeueCdIntr()" };
        }
        case 0xA4: {
            return { "CdGetLbn(filename)" };
        }
        case 0xA5: {
            return { "CdReadSector(count,sector,buffer)" };
        }
        case 0xA6: {
            return { "CdGetStatus()" };
        }
        case 0xA7: {
            return { "bu_callback_okay()" };
        }
        case 0xA8: {
            return { "bu_callback_err_write()" };
        }
        case 0xA9: {
            return { "bu_callback_err_busy()" };
        }
        case 0xAA: {
            return { "bu_callback_err_eject()" };
        }
        case 0xAB: {
            return { "_card_info(port)" };
        }
        case 0xAC: {
            return { "_card_async_load_directory(port)" };
        }
        case 0xAD: {
            return { "set_card_auto_format(flag)" };
        }
        case 0xAE: {
            return { "bu_callback_err_prev_write()" };
        }
        case 0xAF: {
            return { "card_write_test(port)" };
        }
        case 0xB0: {
            return { "return 0" };
        }
        case 0xB1: {
            return { "return 0" };
        }
        case 0xB2: {
            return { "ioabort_raw(param)" };
        }
        case 0xB3: {
            return { "return 0" };
        }
        case 0xB4: {
            return { "GetSystemInfo(index)" };
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
            return { "N/A" };
        }
        default: {
            stringstream ss;
            ss << "Unknown A function with r9: " << std::hex << r9;
            return { ss.str() };
        }
    }
}

optional<string> BIOS::checkBFunctions(uint32_t r9) {
    switch (r9) {
        case 0x00: {
            return { "alloc_kernel_memory(size)" };
        }
        case 0x01: {
            return { "free_kernel_memory(buf)" };
        }
        case 0x02: {
            return { "init_timer(t,reload,flags)" };
        }
        case 0x03: {
            return { "get_timer(t)" };
        }
        case 0x04: {
            return { "enable_timer_irq(t)" };
        }
        case 0x05: {
            return { "disable_timer_irq(t)" };
        }
        case 0x06: {
            return { "restart_timer(t)" };
        }
        case 0x07: {
            return { "DeliverEvent(class, spec)" };
        }
        case 0x08: {
            return { "OpenEvent(class,spec,mode,func)" };
        }
        case 0x09: {
            return { "CloseEvent(event)" };
        }
        case 0x0A: {
            return { "WaitEvent(event)" };
        }
        case 0x0B: {
            return { "TestEvent(event)" };
        }
        case 0x0C: {
            return { "EnableEvent(event)" };
        }
        case 0x0D: {
            return { "DisableEvent(event)" };
        }
        case 0x0E: {
            return { "OpenThread(reg_PC,reg_SP_FP,reg_GP)" };
        }
        case 0x0F: {
            return { "CloseThread(handle)" };
        }
        case 0x10: {
            return { "ChangeThread(handle)" };
        }
        case 0x11: {
            return { "jump_to_00000000h" };
        }
        case 0x12: {
            return { "InitPad(buf1,siz1,buf2,siz2)" };
        }
        case 0x13: {
            return { "StartPad()" };
        }
        case 0x14: {
            return { "StopPad()" };
        }
        case 0x15: {
            return { "OutdatedPadInitAndStart(type,button_dest,unused,unused)" };
        }
        case 0x16: {
            return { "OutdatedPadGetButtons()" };
        }
        case 0x17: {
            return { "ReturnFromException()" };
        }
        case 0x18: {
            return { "SetDefaultExitFromException()" };
        }
        case 0x19: {
            return { "SetCustomExitFromException(addr)" };
        }
        case 0x1A: {
            return { "return 0" };
        }
        case 0x1B: {
            return { "return 0" };
        }
        case 0x1C: {
            return { "return 0" };
        }
        case 0x1D: {
            return { "return 0" };
        }
        case 0x1E: {
            return { "return 0" };
        }
        case 0x1F: {
            return { "return 0" };
        }
        case 0x20: {
            return { "UnDeliverEvent(class,spec)" };
        }
        case 0x21: {
            return { "return 0" };
        }
        case 0x22: {
            return { "return 0" };
        }
        case 0x23: {
            return { "return 0" };
        }
        case 0x24: {
            return { "jump_to_00000000h" };
        }
        case 0x25: {
            return { "jump_to_00000000h" };
        }
        case 0x26: {
            return { "jump_to_00000000h" };
        }
        case 0x27: {
            return { "jump_to_00000000h" };
        }
        case 0x28: {
            return { "jump_to_00000000h" };
        }
        case 0x29: {
            return { "jump_to_00000000h" };
        }
        case 0x2A: {
            return { "return 0" };
        }
        case 0x2B: {
            return { "return 0" };
        }
        case 0x2C: {
            return { "jump_to_00000000h" };
        }
        case 0x2D: {
            return { "jump_to_00000000h" };
        }
        case 0x2E: {
            return { "jump_to_00000000h" };
        }
        case 0x2F: {
            return { "jump_to_00000000h" };
        }
        case 0x30: {
            return { "jump_to_00000000h" };
        }
        case 0x31: {
            return { "jump_to_00000000h" };
        }
        case 0x32: {
            return { "FileOpen(filename,accessmode)" };
        }
        case 0x33: {
            return { "FileSeek(fd,offset,seektype)" };
        }
        case 0x34: {
            return { "FileRead(fd,dst,length)" };
        }
        case 0x35: {
            return { "FileWrite(fd,src,length)" };
        }
        case 0x36: {
            return { "FileClose(fd)" };
        }
        case 0x37: {
            return { "FileIoctl(fd,cmd,arg)" };
        }
        case 0x38: {
            return { "exit(exitcode)" };
        }
        case 0x39: {
            return { "FileGetDeviceFlag(fd)" };
        }
        case 0x3A: {
            return { "FileGetc(fd)" };
        }
        case 0x3B: {
            return { "FilePutc(char,fd)" };
        }
        case 0x3C: {
            return { "std_in_getchar()" };
        }
        case 0x3D: {
            return { "std_out_putchar(char)" };
        }
        case 0x3E: {
            return { "std_in_gets(dst)" };
        }
        case 0x3F: {
            return { "std_out_puts(src)" };
        }
        case 0x40: {
            return { "chdir(name)" };
        }
        case 0x41: {
            return { "FormatDevice(devicename)" };
        }
        case 0x42: {
            return { "firstfile(filename,direntry)" };
        }
        case 0x43: {
            return { "nextfile(direntry)" };
        }
        case 0x44: {
            return { "FileRename(old_filename,new_filename)" };
        }
        case 0x45: {
            return { "FileDelete(filename)" };
        }
        case 0x46: {
            return { "FileUndelete(filename)" };
        }
        case 0x47: {
            return { "AddDevice(device_info)" };
        }
        case 0x48: {
            return { "RemoveDevice(device_name_lowercase)" };
        }
        case 0x49: {
            return { "PrintInstalledDevices()" };
        }
        case 0x4A: {
            return { "InitCard(pad_enable)" };
        }
        case 0x4B: {
            return { "StartCard()" };
        }
        case 0x4C: {
            return { "StopCard()" };
        }
        case 0x4D: {
            return { "_card_info_subfunc(port)" };
        }
        case 0x4E: {
            return { "write_card_sector(port,sector,src)" };
        }
        case 0x4F: {
            return { "read_card_sector(port,sector,dst)" };
        }
        case 0x50: {
            return { "allow_new_card()" };
        }
        case 0x51: {
            return { "Krom2RawAdd(shiftjis_code)" };
        }
        case 0x52: {
            return { "SystemError" };
        }
        case 0x53: {
            return { "Krom2Offset(shiftjis_code)" };
        }
        case 0x54: {
            return { "GetLastError()" };
        }
        case 0x55: {
            return { "GetLastFileError(fd)" };
        }
        case 0x56: {
            return { "GetC0Table" };
        }
        case 0x57: {
            return { "GetB0Table" };
        }
        case 0x58: {
            return { "get_bu_callback_port()" };
        }
        case 0x59: {
            return { "testdevice(devicename)" };
        }
        case 0x5A: {
            return { "SystemError" };
        }
        case 0x5B: {
            return { "ChangeClearPad(int)" };
        }
        case 0x5C: {
            return { "get_card_status(slot)" };
        }
        case 0x5D: {
            return { "wait_card_status(slot)" };
        }
        case 0x5E: {
            return { "jump_to_00000000h" };
        }
        default: {
            stringstream ss;
            ss << "Unknown B function with r9: " << std::hex << r9;
            return { ss.str() };
        }
    }
}

optional<string> BIOS::checkCFunctions(uint32_t r9) {
    switch (r9) {
        case 0x00: {
            return { "EnqueueTimerAndVblankIrqs(priority)" };
        }
        case 0x01: {
            return { "EnqueueSyscallHandler(priority)" };
        }
        case 0x02: {
            return { "SysEnqIntRP(priority,struc)" };
        }
        case 0x03: {
            return { "SysDeqIntRP(priority,struc)" };
        }
        case 0x04: {
            return { "get_free_EvCB_slot()" };
        }
        case 0x05: {
            return { "get_free_TCB_slot()" };
        }
        case 0x06: {
            return { "ExceptionHandler()" };
        }
        case 0x07: {
            return { "InstallExceptionHandlers()" };
        }
        case 0x08: {
            return { "SysInitMemory(addr,size)" };
        }
        case 0x09: {
            return { "SysInitKernelVariables()" };
        }
        case 0x0A: {
            return { "ChangeClearRCnt(t,flag)" };
        }
        case 0x0B: {
            return { "SystemError" };
        }
        case 0x0C: {
            return { "InitDefInt(priority)" };
        }
        case 0x0D: {
            return { "SetIrqAutoAck(irq,flag)" };
        }
        case 0x0E: {
            return { "dev_sio_init" };
        }
        case 0x0F: {
            return { "dev_sio_open" };
        }
        case 0x10: {
            return { "dev_sio_in_out" };
        }
        case 0x11: {
            return { "dev_sio_ioctl" };
        }
        case 0x12: {
            return { "InstallDevices(ttyflag)" };
        }
        case 0x13: {
            return { "FlushStdInOutPut()" };
        }
        case 0x14: {
            return { "SystemError" };
        }
        case 0x15: {
            return { "tty_cdevinput(circ,char)" };
        }
        case 0x16: {
            return { "tty_cdevscan()" };
        }
        case 0x17: {
            return { "tty_circgetc(circ)" };
        }
        case 0x18: {
            return { "tty_circputc(char,circ)" };
        }
        case 0x19: {
            return { "ioabort(txt1,txt2)" };
        }
        case 0x1A: {
            return { "set_card_find_mode(mode)" };
        }
        case 0x1B: {
            return { "KernelRedirect(ttyflag)" };
        }
        case 0x1C: {
            return { "AdjustA0Table()" };
        }
        case 0x1D: {
            return { "get_card_find_mode()" };
        }
        default: {
            stringstream ss;
            ss << "Unknown C function with r9: " << std::hex << r9;
            return { ss.str() };
        }
    }
}

optional<string> BIOS::checkFunctions(uint32_t programCounter, uint32_t r9) {
    switch (programCounter) {
        case BIOS_A_FUNCTIONS_STEP: {
            return checkAFunctions(r9);
        }
        case BIOS_B_FUNCTIONS_STEP: {
            return checkBFunctions(r9);
        }
        case BIOS_C_FUNCTIONS_STEP: {
            return checkCFunctions(r9);
        }
        default: {
            return { nullopt };
        }
    }
}
