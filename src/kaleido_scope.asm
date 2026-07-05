//save warp fix

.orga 0xBC5154
addiu	a0, s1, 0x0000
jal     save_game

.orga 0xBC5F10
addiu	a0, s1, 0x0000
jal     save_game

//FileSelect_LoadGame intercept Sram_OpenSave
.orga 0xBAEC0C
    jal     sram_open_save_wrapper

//enable mirror shield for child link
.orga 0xBC77B3
.byte 0x09

.orga 0xBC77FC
.byte 0x09

//enable tunics for child link
.orga 0xBC77B6
.byte 0x09, 0x09

.orga 0xBC77FE
.byte 0x09, 0x09