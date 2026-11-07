#pragma once
#include "common.h"

namespace drv::sdCard {

union __packed RegCSDv1 {
    struct {
        uint32 reserved0           : 1;  // bit 0, always 1
        uint32 crc                 : 7;  // bits 7:1
        uint32 reserved1           : 2;  // bits 9:8
        uint32 file_format         : 2;  // bits 11:10
        uint32 tmp_write_protect   : 1;  // bit 12
        uint32 perm_write_protect  : 1;  // bit 13
        uint32 copy                : 1;  // bit 14
        uint32 file_format_grp     : 1;  // bit 15

        uint32 reserved2           : 5;  // bits 20:16
        uint32 write_bl_partial    : 1;  // bit 21
        uint32 write_bl_len        : 4;  // bits 25:22
        uint32 r2w_factor          : 3;  // bits 28:26
        uint32 reserved3           : 2;  // bits 30:29
        uint32 wp_grp_enable       : 1;  // bit 31

        uint32 wp_grp_size         : 7;  // bits 38:32
        uint32 sector_size         : 7;  // bits 45:39
        uint32 erase_blk_en        : 1;  // bit 46
        uint32 c_size_mult         : 3;  // bits 49:47
        uint32 vdd_w_curr_max      : 3;  // bits 52:50
        uint32 vdd_w_curr_min      : 3;  // bits 55:53
        uint32 vdd_r_curr_max      : 3;  // bits 58:56
        uint32 vdd_r_curr_min      : 3;  // bits 61:59

        uint32 c_size              : 12; // lower bits of bits 73:62
        uint32 c_size_hi           : 10; // upper bits of bits 79:74
        uint32 reserved4           : 4;  // bits 83:80
        uint32 dsr_imp             : 1;  // bit 84
        uint32 read_blk_misalign   : 1;  // bit 85
        uint32 write_blk_misalign  : 1;  // bit 86
        uint32 read_bl_partial     : 1;  // bit 87

        uint32 read_bl_len         : 4;  // bits 91:88
        uint32 ccc                 : 12; // bits 103:92
        uint32 tran_speed          : 8;  // bits 111:104
        uint32 nsac                : 8;  // bits 119:112
        uint32 taac                : 8;  // bits 127:120
    } bit;
    uint8 val[16];
};

union __packed RegCSDv2 {
    struct {
        uint32 RESERVED0            : 1;
        uint32 CRC                  : 7;
        uint32 RESERVED1            : 2;
        uint32 FILE_FORMAT          : 2;
        uint32 TMP_WRITE_PROTECT    : 1;
        uint32 PERM_WRITE_PROTECT   : 1;
        uint32 COPY                 : 1;
        uint32 FILE_FORMAT_GRP      : 1;
        uint32 RESERVED2            : 5;
        uint32 WRITE_BL_PARTIAL     : 1;
        uint32 WRITE_BL_LEN         : 4;
        uint32 R2W_FACTOR           : 3;
        uint32 RESERVED3            : 2;
        uint32 WP_GRP_ENABLE        : 1;

        uint32 WP_GRP_SIZE          : 7;
        uint32 SECTOR_SIZE          : 7;
        uint32 ERASE_BLK_EN         : 1;
        uint32 RESERVED4            : 1;
        uint32 C_SIZE_LO            : 16;

        uint32 C_SIZE_HI            : 6;
        uint32 RESERVED5            : 6;
        uint32 DSR_IMP              : 1;
        uint32 READ_BLK_MISALIGN    : 1;
        uint32 WRITE_BLK_MISALIGN   : 1;
        uint32 READ_BL_PARTIAL      : 1;
        uint32 READ_BL_LEN          : 4;
        uint32 CCC                  : 12;

        uint32 TRAN_SPEED           : 8;
        uint32 NSAC                 : 8;
        uint32 TAAC                 : 8;
        uint32 RESERVED6            : 6;
        uint32 CSD_STRUCTURE        : 2;
    } bit;
    uint8 val[16];
};

} // End of namespace 'drv::sdCard'