/* Linux settings */
#define DTB_ADDR         (0x20000000)
#define ZIMAGE_ADDR      (0x20008000)
#define INITRAMFS_ADDR   (0x27800000)
#define MACHINE_NUMBER   (0xFFFFFFFF)
#define ARM9LINUXFW_ADDR (0x08080000)
#define SYNC_ADDR        (0x1FFFFFF0)
#define SYNC_INIT        (0)
#define SYNC_FB_RDY      (1)
#define SYNC_BOOT_RDY    (2)

#define LINUXIMAGE_FILENAME  "linux/zImage"
#define INITRAMFS_FILENAME   "linux/initramfs.cpio.gz"
#define CTR_DTB_FILENAME     "linux/nintendo3ds_ctr.dtb"
#define KTR_DTB_FILENAME     "linux/nintendo3ds_ktr.dtb"
#define ARM9LINUXFW_FILENAME "linux/arm9linuxfw.bin"

/* 3DS memory layout */
#define VRAM_BASE     (0x18000000)
#define VRAM_SIZE     (0x00600000)
#define AXI_WRAM_BASE (0x1FF80000)
#define AXI_WRAM_SIZE (0x00080000)
#define FCRAM_BASE    (0x20000000)
#define FCRAM_SIZE    (0x08000000)

/* LCD Frambuffers stuff (Physical Addresses) */
#define LCD_FB_PDC0           (0x10400400)
#define LCD_FB_PDC1           (0x10400500)
#define LCD_FB_A_ADDR_OFFSET  (0x68)
#define LCD_FB_FORMAT_OFFSET  (0x70)
#define LCD_FB_PDC0_FORMAT    (0x80342)
#define LCD_FB_SELECT_OFFSET  (0x78)
#define LCD_FB_STRIDE_OFFSET  (0x90)
#define LCD_FB_PDC0_STRIDE    (0x1E0)
#define LCD_FB_B_ADDR_OFFSET  (0x94)
#define FB_TOP_SIZE           (400 * 240 * 2)
#define FB_BOT_SIZE           (320 * 240 * 3)
#define FB_BASE_PA            (VRAM_BASE)
#define FB_TOP_LEFT1          (FB_BASE_PA)
#define FB_TOP_LEFT2          (FB_TOP_LEFT1  + FB_TOP_SIZE)
#define FB_TOP_RIGHT1         (FB_TOP_LEFT2  + FB_TOP_SIZE)
#define FB_TOP_RIGHT2         (FB_TOP_RIGHT1 + FB_TOP_SIZE)
#define FB_BOT_1              (FB_TOP_RIGHT2 + FB_BOT_SIZE)
#define FB_BOT_2              (FB_BOT_1      + FB_BOT_SIZE)