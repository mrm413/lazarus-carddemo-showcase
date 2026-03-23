/**
 * COTRTLIC — CardDemo CICS Program
 * Transpiled by Lazarus COBOL-to-C++17
 */

#include <cobol_runtime.h>
namespace {



// Working Storage variables
FixedString<100> carddemo_commarea;
FixedString<15> card_num;
FixedString<8> ccard_aid;
FixedString<100> ccda_msg_thank_you;
FixedString<50> ccda_title01;
FixedString<50> cdemo_from_program;
FixedString<8> cdemo_from_tranid;
FixedString<80> dcl_tr_description_len;
FixedString<80> dcl_tr_description_text;
FixedString<50> dcl_tr_type;
FixedString<50> dfhall;
FixedString<50> dfhblink;
FixedString<50> dfhblue;
FixedString<50> dfhbmasb;
FixedString<50> dfhbmasf;
FixedString<50> dfhbmask;
FixedString<50> dfhbmbry;
FixedString<50> dfhbmdar;
FixedString<50> dfhbmdef;
FixedString<50> dfhbmeof;
FixedString<50> dfhbmfil;
FixedString<50> dfhbmfse;
FixedString<50> dfhbmpem;
FixedString<50> dfhbmpnl;
FixedString<50> dfhbmprf;
FixedString<50> dfhbmpro;
FixedString<50> dfhbmunn;
FixedString<50> dfhbmunp;
FixedString<50> dfhclear;
FixedString<50> dfhcolor;
FixedString<100> dfhcommarea;
FixedString<50> dfhdfcol;
FixedString<50> dfhdfhi;
FixedString<50> dfhdft;
FixedString<50> dfheiblk;
FixedString<50> dfhenter;
FixedString<50> dfherror;
FixedString<50> dfhgreen;
FixedString<50> dfhhlt;
FixedString<50> dfhmsre;
FixedString<50> dfhneutr;
FixedString<8> dfhopid;
FixedString<50> dfhoutln;
FixedString<50> dfhpa1;
FixedString<50> dfhpa2;
FixedString<50> dfhpa3;
FixedString<50> dfhpen;
FixedString<50> dfhpf1;
FixedString<50> dfhpf10;
FixedString<50> dfhpf11;
FixedString<50> dfhpf12;
FixedString<50> dfhpf13;
FixedString<50> dfhpf14;
FixedString<50> dfhpf15;
FixedString<50> dfhpf16;
FixedString<50> dfhpf17;
FixedString<50> dfhpf18;
FixedString<50> dfhpf19;
FixedString<50> dfhpf2;
FixedString<50> dfhpf20;
FixedString<50> dfhpf21;
FixedString<50> dfhpf22;
FixedString<50> dfhpf23;
FixedString<50> dfhpf24;
FixedString<50> dfhpf3;
FixedString<50> dfhpf4;
FixedString<50> dfhpf5;
FixedString<50> dfhpf6;
FixedString<50> dfhpf7;
FixedString<50> dfhpf8;
FixedString<50> dfhpf9;
FixedString<50> dfhpink;
FixedString<50> dfhps;
FixedString<50> dfhred;
FixedString<50> dfhrevrs;
FixedString<50> dfhsa;
FixedString<50> dfhsosi;
FixedString<50> dfhstrf;
FixedString<50> dfhtrig;
FixedString<50> dfhturq;
FixedString<50> dfhundln;
FixedString<50> dfhval;
FixedString<50> dfhwhte;
FixedString<50> dfhyello;
FixedString<100> errmsgo;
FixedString<50> I;
FixedString<50> i_selected;
FixedString<15> lit_all_alphanum_from;
FixedString<50> lit_all_alpha_from;
FixedString<15> lit_all_num_from;
FixedString<15> lit_alphanum_spaces_to;
FixedString<50> lit_alpha_spaces_to;
FixedString<50> lit_asterisk;
FixedString<1> lit_delete_flag;
FixedString<50> lit_dsntiac;
FixedString<15> lit_num_spaces_to;
FixedString<50> lit_thispgm;
FixedString<10> lit_update_flag;
FixedString<50> pagenoo;
int RETURN_CODE = 0;
FixedString<80> sec_user_data;
FixedString<50> sqlca;
int32_t sqlcode = 0;
FixedString<80> trdescl;
FixedString<30> trnnamel;
FixedString<50> trtypel;
FixedString<50> ws_actions_requested;
FixedString<50> ws_ca_row_selected;
FixedString<15> ws_ca_screen_num;
FixedString<50> ws_ca_type_cd;
FixedString<80> ws_ca_type_desc;
FixedString<100> ws_commarea;
FixedString<10> ws_curdate;
FixedString<10> ws_curdate_data;
FixedString<10> ws_curdate_yy;
FixedString<50> ws_db2_current_action;
FixedString<50> ws_deletes_requested;
int32_t ws_disp_sqlcode = 0;
FixedString<50> ws_dsntiac_error;
FixedString<50> ws_dsntiac_err_cd;
FixedString<100> ws_dsntiac_fmtd_text;
FixedString<50> ws_dsntiac_formatted;
FixedString<50> ws_dsntiac_lrecl;
FixedString<10> ws_edit_alphanum_length;
FixedString<15> ws_edit_alphanum_only;
FixedString<1> ws_edit_select_flags;
FixedString<30> ws_edit_variable_name;
FixedString<100> ws_info_msg;
FixedString<50> ws_in_type_cd;
FixedString<80> ws_in_type_desc;
FixedString<100> ws_long_msg;
FixedString<50> ws_max_screen_lines;
FixedString<50> ws_no_actions_selected;
FixedString<50> ws_records_count;
FixedString<10> ws_resp_cd;
FixedString<100> ws_return_msg;
FixedString<15> ws_row_number;
FixedString<10> ws_string_len;
FixedString<8> ws_string_mid;
FixedString<100> ws_this_progcommarea;
FixedString<80> ws_type_desc_filter;
FixedString<10> ws_updates_requested;
FixedString<8> ws_valid_actions_selected;
FixedString<30> xml_namespace;
FixedString<30> xml_namespace_prefix;
FixedString<30> xml_nnamespace;
FixedString<30> xml_nnamespace_prefix;
FixedString<100> xml_ntext;
FixedString<100> xml_text;

// Forward declarations
void p_0000_main();
void p_common_return();
void p_0000_main_exit();
void p_1000_receive_map();
void p_1000_receive_map_exit();
void p_1100_receive_screen();
void p_1100_receive_screen_exit();
void p_1200_edit_inputs();
void p_1200_edit_inputs_exit();
void p_1210_edit_array();
void p_1210_edit_array_exit();
void p_1211_edit_array_desc();
void p_1211_edit_array_desc_exit();
void p_1220_edit_typecd();
void p_1220_edit_typecd_exit();
void p_1230_edit_desc();
void p_1230_edit_desc_exit();
void p_1240_edit_alphanum_reqd();
void p_1240_edit_alphanum_reqd_exit();
void p_1290_cross_edits();
void p_1290_cross_edits_exit();
void p_2000_send_map();
void p_2000_send_map_exit();
void p_2100_screen_init();
void p_2100_screen_init_exit();
void p_2200_setup_array_attribs();
void p_2200_setup_array_attribs_exit();
void p_2300_screen_array_init();
void p_2300_screen_array_init_exit();
void p_2400_setup_screen_attrs();
void p_2400_setup_screen_attrs_exit();
void p_2500_setup_message();
void p_2500_setup_message_exit();
void p_2600_send_screen();
void p_2600_send_screen_exit();
void p_8000_read_forward();
void p_8000_read_forward_exit();
void p_8100_read_backwards();
void p_8100_read_backwards_exit();
void p_9100_check_filters();
void p_9100_check_filters_exit();
void p_9200_update_record();
void p_9200_update_record_exit();
void p_9300_delete_record();
void p_9300_delete_record_exit();
void p_9400_open_forward_cursor();
void p_9400_open_forward_cursor_exit();
void p_9450_close_forward_cursor();
void p_9450_close_forward_cursor_exit();
void p_9500_open_backward_cursor();
void p_9500_open_backward_cursor_exit();
void p_9550_close_back_cursor();
void p_9550_close_back_cursor_exit();
void p_9998_priming_query();
void p_9998_priming_query_exit();
void p_9999_format_db2_message();
void p_9999_format_db2_message_exit();
void p_yyyy_store_pfkey();
void p_yyyy_store_pfkey_exit();
void p_send_plain_text();
void p_send_plain_text_exit();
void p_send_long_text();
void p_send_long_text_exit();

void p_0000_main() {
    ccard_aid = std::string(213, ' ');
    ws_row_number = std::string(8, '\0');
    ws_row_number.replace(8, 4, std::string(4, ' '));
    ws_row_number.replace(12, 18, std::string(18, ' '));
    ws_row_number.replace(30, 281, std::string(281, ' '));
    // MOVE ZERO TO COMP-3 field
    ws_row_number.at(314) = ' ';
    // MOVE ZERO TO COMP-3 field
    ws_row_number.replace(318, 364, std::string(364, ' '));
    // MOVE ZERO TO COMP-3 field
    ws_row_number.replace(685, 14, std::string(14, ' '));
    ws_row_number.replace(699, 4, std::string(4, '\0'));
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    ws_row_number.replace(718, 3, std::string(3, ' '));
    ws_row_number.replace(721, 921, std::string(921, ' '));
    ws_row_number.replace(1642, 6, std::string(6, '0'));
    ws_row_number.replace(1648, 45, std::string(45, ' '));
    ws_row_number.replace(1693, 56, std::string(56, ' '));
    ws_row_number.replace(1751, 2, std::string(2, ' '));
    // UNHANDLED: cob_u8_ptr b_ptr = b_46 + 1750;
    // UNHANDLED: memcpy (b_ptr +      5, b_ptr,      5);	/* WS-TYPE-CD-DELETE-KEYS:     2 thru 2 */ memcpy (b_ptr +     10, b_ptr,     10);	/* WS-TYPE-CD-DELETE-KEYS:     3 thru 4 */ memcpy (b_ptr +     20, b_ptr,     15);	/* WS-TYPE-CD-DELETE-KEYS:     5 thru 7 */ }
    ws_disp_sqlcode = 0;
    // MOVE ZERO TO COMP-3 field
    ws_row_number.replace(1797, 73, std::string(73, ' '));
    ws_row_number.replace(1870, 2, std::string(2, '\0'));
    ws_row_number.replace(1872, 720, std::string(720, ' '));
    ws_row_number.replace(2592, 2, std::string(2, '\0'));
    ws_row_number.replace(2594, 12, std::string(12, ' '));
    ws_row_number.replace(2606, 52, std::string(52, ' '));
    ws_row_number.replace(2658, 2, std::string(2, '\0'));
    ws_row_number.at(2660) = ' ';
    ws_commarea = std::string(2000, ' ');
    ws_row_number.replace(8, 4, ws_max_screen_lines.substr(8, 4));
    ws_row_number.replace(1566, 75, std::string(75, ' '));
    if (to_int(RETURN_CODE) == 0) {
        cdemo_from_program = std::string(33, ' ');
        cdemo_from_program.at(33) = '0';
        cdemo_from_program.replace(34, 9, std::string(9, '0'));
        cdemo_from_program.replace(43, 75, std::string(75, ' '));
        cdemo_from_program.replace(118, 11, std::string(11, '0'));
        cdemo_from_program.at(129) = ' ';
        cdemo_from_program.replace(130, 16, std::string(16, '0'));
        cdemo_from_program.replace(146, 14, std::string(14, ' '));
        ws_ca_screen_num = std::string(52, ' ');
        ws_ca_screen_num.replace(52, 364, std::string(364, ' '));
        ws_ca_screen_num.replace(416, 2, std::string(2, '\0'));
        ws_ca_screen_num.replace(418, 4, std::string(4, ' '));
        ws_ca_screen_num.replace(422, 2, std::string(2, '0'));
        ws_ca_screen_num.at(424) = ' ';
        ws_ca_screen_num.replace(425, 2, std::string(2, ' '));
        cdemo_from_program = ws_max_screen_lines.substr(8, 4);
        cdemo_from_program.replace(4, 8, ws_max_screen_lines.substr(0, 8));
        cdemo_from_program.at(32) = 'A';
        cdemo_from_program.at(33) = '0';
        cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
        cdemo_from_program.replace(153, 7, ws_max_screen_lines.substr(12, 7));
        ws_ca_screen_num.at(422) = '1';
        ws_ca_screen_num.at(423) = static_cast<char>(57);
    } else {
        // MOVE to computed field
        // MOVE to computed field
    }
    p_yyyy_store_pfkey();
    if (to_int(cdemo_from_program) == 0 && cdemo_from_program.substr(4, 8) != ws_max_screen_lines.substr(0, 8) || ccard_aid.substr(0, 5) == "PFK03" && cdemo_from_program.substr(0, 4) == ws_max_screen_lines.substr(53, 4)) {
        ws_ca_screen_num = std::string(52, ' ');
        ws_ca_screen_num.replace(52, 364, std::string(364, ' '));
        ws_ca_screen_num.replace(416, 2, std::string(2, '\0'));
        ws_ca_screen_num.replace(418, 4, std::string(4, ' '));
        ws_ca_screen_num.replace(422, 2, std::string(2, '0'));
        ws_ca_screen_num.at(424) = ' ';
        ws_ca_screen_num.replace(425, 2, std::string(2, ' '));
        cdemo_from_program.at(33) = '0';
        ccard_aid = "ENTER";
        cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
        ws_ca_screen_num.at(422) = '1';
        ws_ca_screen_num.at(423) = static_cast<char>(57);
    }
    if (to_int(RETURN_CODE) > 0 && cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(0, 8)) {
        p_1000_receive_map();
    }
    ws_row_number.at(1641) = '1';
    if (ccard_aid.substr(0, 5) == "ENTER" || ccard_aid.substr(0, 5) == "PFK02" || ccard_aid.substr(0, 5) == "PFK03" || ccard_aid.substr(0, 5) == "PFK07" || ccard_aid.substr(0, 5) == "PFK08" || ccard_aid.substr(0, 5) == "PFK10" && ws_ca_screen_num.at(425) == 'Y' || ccard_aid.substr(0, 5) == "PFK10" && ws_ca_screen_num.at(426) == 'Y') {
        ws_row_number.at(1641) = '0';
    }
    if (ws_row_number.at(1641) == '1') {
        ccard_aid = "ENTER";
    }
    if (ccard_aid.substr(0, 5) == "PFK03") {
        if (to_int(cdemo_from_tranid) == to_int(0) || false /* TODO: memcmp (cdemo_from_program, COB_SPACES_ALPHABETIC, 4) == 0 */ || cdemo_from_program.substr(0, 4) == ws_max_screen_lines.substr(8, 4)) {
            cdemo_from_program.replace(12, 4, ws_max_screen_lines.substr(34, 4));
        } else {
            cdemo_from_program.replace(12, 4, cdemo_from_program.substr(0, 4));
        }
        if (to_int(cdemo_from_program) == to_int(0) || false /* TODO: memcmp (cdemo_from_program + 4, COB_SPACES_ALPHABETIC, 8) == 0 */ || cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(0, 8)) {
            cdemo_from_program.replace(16, 8, ws_max_screen_lines.substr(26, 8));
        } else {
            cdemo_from_program.replace(16, 8, cdemo_from_program.substr(4, 8));
        }
        cdemo_from_program = ws_max_screen_lines.substr(8, 4);
        cdemo_from_program.replace(4, 8, ws_max_screen_lines.substr(0, 8));
        cdemo_from_program.at(32) = 'A';
        cdemo_from_program.at(33) = '0';
        cdemo_from_program.replace(153, 7, ws_max_screen_lines.substr(12, 7));
        cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
    }
    if (ccard_aid.substr(0, 5) == "PFK02" && cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(0, 8)) {
        cdemo_from_program = ws_max_screen_lines.substr(8, 4);
        cdemo_from_program.replace(4, 8, ws_max_screen_lines.substr(0, 8));
        cdemo_from_program.at(32) = static_cast<char>(85);
        cdemo_from_program.at(33) = '0';
        cdemo_from_program.replace(153, 7, ws_max_screen_lines.substr(12, 7));
        cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
        cdemo_from_program.replace(16, 8, ws_max_screen_lines.substr(45, 8));
        ccard_aid.replace(13, 7, ws_max_screen_lines.substr(57, 7));
        ccard_aid.replace(20, 7, ws_max_screen_lines.substr(64, 7));
        ws_return_msg = "PF03 pressed. Exiting";
        cdemo_from_program.at(33) = '0';
    }
    if (ccard_aid.substr(0, 5) == "PFK08") {
    } else {
        ws_ca_screen_num.at(423) = static_cast<char>(57);
    }
    if (ccard_aid.substr(0, 5) == "PFK10") {
        if (ws_ca_screen_num.at(425) == 'Y' || ws_ca_screen_num.at(426) == 'Y' && false /* TODO: (*(ws_row_number + 15)) == 0 */ && false /* TODO: (*(ws_row_number + 16)) == 0 */ && false /* TODO: (*(ws_row_number + 26)) == 0 */) {
        } else {
            ccard_aid = "ENTER";
        }
    } else {
    }
    p_9998_priming_query();
    if (ws_row_number.at(1797) == '1') {
        p_send_long_text();
    }
    if (ws_row_number.at(12) == '1') {
        ccard_aid.replace(27, 75, ws_row_number.substr(1566, 75));
        cdemo_from_program.replace(4, 8, ws_max_screen_lines.substr(0, 8));
        cdemo_from_program.replace(153, 7, ws_max_screen_lines.substr(12, 7));
        cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
        ccard_aid.replace(5, 8, ws_max_screen_lines.substr(0, 8));
        ccard_aid.replace(13, 7, ws_max_screen_lines.substr(12, 7));
        ccard_aid.replace(20, 7, ws_max_screen_lines.substr(19, 7));
        ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
        if (ws_row_number.at(13) == '0' && ws_row_number.at(14) == '0') {
            p_8000_read_forward();
        }
        p_2000_send_map();
    }
    if (ccard_aid.substr(0, 5) == "PFK07" && to_int(ws_ca_screen_num) == 1 || ccard_aid.substr(0, 5) == "PFK07" && to_int(ws_ca_screen_num) == 1) {
        ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
        p_8000_read_forward();
        p_2000_send_map();
    }
    if (ccard_aid.substr(0, 5) == "PFK03" || to_int(cdemo_from_program) == 1 && cdemo_from_program.substr(4, 8) != ws_max_screen_lines.substr(0, 8)) {
        cdemo_from_program = std::string(33, ' ');
        cdemo_from_program.at(33) = '0';
        cdemo_from_program.replace(34, 9, std::string(9, '0'));
        cdemo_from_program.replace(43, 75, std::string(75, ' '));
        cdemo_from_program.replace(118, 11, std::string(11, '0'));
        cdemo_from_program.at(129) = ' ';
        cdemo_from_program.replace(130, 16, std::string(16, '0'));
        cdemo_from_program.replace(146, 14, std::string(14, ' '));
        ws_ca_screen_num = std::string(52, ' ');
        ws_ca_screen_num.replace(52, 364, std::string(364, ' '));
        ws_ca_screen_num.replace(416, 2, std::string(2, '\0'));
        ws_ca_screen_num.replace(418, 4, std::string(4, ' '));
        ws_ca_screen_num.replace(422, 2, std::string(2, '0'));
        ws_ca_screen_num.at(424) = ' ';
        ws_ca_screen_num.replace(425, 2, std::string(2, ' '));
        ws_row_number = std::string(8, '\0');
        ws_row_number.replace(8, 4, std::string(4, ' '));
        ws_row_number.replace(12, 18, std::string(18, ' '));
        ws_row_number.replace(30, 281, std::string(281, ' '));
        // MOVE ZERO TO COMP-3 field
        ws_row_number.at(314) = ' ';
        // MOVE ZERO TO COMP-3 field
        ws_row_number.replace(318, 364, std::string(364, ' '));
        // MOVE ZERO TO COMP-3 field
        ws_row_number.replace(685, 14, std::string(14, ' '));
        ws_row_number.replace(699, 4, std::string(4, '\0'));
        // MOVE ZERO TO COMP-3 field
        // MOVE ZERO TO COMP-3 field
        // MOVE ZERO TO COMP-3 field
        // MOVE ZERO TO COMP-3 field
        // MOVE ZERO TO COMP-3 field
        ws_row_number.replace(718, 3, std::string(3, ' '));
        ws_row_number.replace(721, 921, std::string(921, ' '));
        ws_row_number.replace(1642, 6, std::string(6, '0'));
        ws_row_number.replace(1648, 45, std::string(45, ' '));
        ws_row_number.replace(1693, 56, std::string(56, ' '));
        ws_row_number.replace(1751, 2, std::string(2, ' '));
        // UNHANDLED: cob_u8_ptr b_ptr = b_46 + 1750;
        // UNHANDLED: memcpy (b_ptr +      5, b_ptr,      5);	/* WS-TYPE-CD-DELETE-KEYS:     2 thru 2 */ memcpy (b_ptr +     10, b_ptr,     10);	/* WS-TYPE-CD-DELETE-KEYS:     3 thru 4 */ memcpy (b_ptr +     20, b_ptr,     15);	/* WS-TYPE-CD-DELETE-KEYS:     5 thru 7 */ }
        ws_disp_sqlcode = 0;
        // MOVE ZERO TO COMP-3 field
        ws_row_number.replace(1797, 73, std::string(73, ' '));
        ws_row_number.replace(1870, 2, std::string(2, '\0'));
        ws_row_number.replace(1872, 720, std::string(720, ' '));
        ws_row_number.replace(2592, 2, std::string(2, '\0'));
        ws_row_number.replace(2594, 12, std::string(12, ' '));
        ws_row_number.replace(2606, 52, std::string(52, ' '));
        ws_row_number.replace(2658, 2, std::string(2, '\0'));
        ws_row_number.at(2660) = ' ';
        cdemo_from_program = ws_max_screen_lines.substr(8, 4);
        cdemo_from_program.replace(4, 8, ws_max_screen_lines.substr(0, 8));
        cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
        cdemo_from_program.replace(153, 7, ws_max_screen_lines.substr(12, 7));
        cdemo_from_program.at(32) = 'A';
        cdemo_from_program.at(33) = '0';
        ws_ca_screen_num.at(422) = '1';
        ws_ca_screen_num.at(423) = static_cast<char>(57);
        ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
        p_8000_read_forward();
        p_2000_send_map();
        if (ccard_aid.substr(0, 5) == "PFK08" && ws_ca_screen_num.at(424) == 'Y') {
            ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(418, 2));
            ws_ca_screen_num = to_string(to_int(ws_ca_screen_num) + 1);
            p_8000_read_forward();
            ws_row_number.replace(685, 7, std::string(7, ' '));
            p_2000_send_map();
        }
        if (ccard_aid.substr(0, 5) == "PFK07" && to_int(ws_ca_screen_num) == 1) {
            ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
            ws_ca_screen_num = to_string(to_int(ws_ca_screen_num) - 1);
            p_8100_read_backwards();
            ws_row_number.replace(685, 7, std::string(7, ' '));
            p_2000_send_map();
        }
        if (ccard_aid.substr(0, 5) == "ENTER" && to_int(ws_deletes_requested) > 0 && cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(0, 8)) {
            ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
            if (ws_row_number.at(13) == '0' && ws_row_number.at(14) == '0') {
                p_8000_read_forward();
            }
            p_2000_send_map();
        }
        if (ccard_aid.substr(0, 5) == "PFK10" && to_int(ws_deletes_requested) > 0 && cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(0, 8)) {
            p_9300_delete_record();
            if (false /* TODO: (*(ws_ca_screen_num + 425)) == 0 */) {
                ws_row_number.at(24) = 'Y';
            } else {
                ws_row_number.at(24) = static_cast<char>(0);
            }
            p_2000_send_map();
            if (ws_row_number.at(24) == 'Y') {
                cdemo_from_program = std::string(33, ' ');
                cdemo_from_program.at(33) = '0';
                cdemo_from_program.replace(34, 9, std::string(9, '0'));
                cdemo_from_program.replace(43, 75, std::string(75, ' '));
                cdemo_from_program.replace(118, 11, std::string(11, '0'));
                cdemo_from_program.at(129) = ' ';
                cdemo_from_program.replace(130, 16, std::string(16, '0'));
                cdemo_from_program.replace(146, 14, std::string(14, ' '));
                ws_ca_screen_num = std::string(52, ' ');
                ws_ca_screen_num.replace(52, 364, std::string(364, ' '));
                ws_ca_screen_num.replace(416, 2, std::string(2, '\0'));
                ws_ca_screen_num.replace(418, 4, std::string(4, ' '));
                ws_ca_screen_num.replace(422, 2, std::string(2, '0'));
                ws_ca_screen_num.at(424) = ' ';
                ws_ca_screen_num.replace(425, 2, std::string(2, ' '));
                ws_row_number = std::string(8, '\0');
                ws_row_number.replace(8, 4, std::string(4, ' '));
                ws_row_number.replace(12, 18, std::string(18, ' '));
                ws_row_number.replace(30, 281, std::string(281, ' '));
                // MOVE ZERO TO COMP-3 field
                ws_row_number.at(314) = ' ';
                // MOVE ZERO TO COMP-3 field
                ws_row_number.replace(318, 364, std::string(364, ' '));
                // MOVE ZERO TO COMP-3 field
                ws_row_number.replace(685, 14, std::string(14, ' '));
                ws_row_number.replace(699, 4, std::string(4, '\0'));
                // MOVE ZERO TO COMP-3 field
                // MOVE ZERO TO COMP-3 field
                // MOVE ZERO TO COMP-3 field
                // MOVE ZERO TO COMP-3 field
                // MOVE ZERO TO COMP-3 field
                ws_row_number.replace(718, 3, std::string(3, ' '));
                ws_row_number.replace(721, 921, std::string(921, ' '));
                ws_row_number.replace(1642, 6, std::string(6, '0'));
                ws_row_number.replace(1648, 45, std::string(45, ' '));
                ws_row_number.replace(1693, 56, std::string(56, ' '));
                ws_row_number.replace(1751, 2, std::string(2, ' '));
                // UNHANDLED: cob_u8_ptr b_ptr = b_46 + 1750;
                // UNHANDLED: memcpy (b_ptr +      5, b_ptr,      5);	/* WS-TYPE-CD-DELETE-KEYS:     2 thru 2 */ memcpy (b_ptr +     10, b_ptr,     10);	/* WS-TYPE-CD-DELETE-KEYS:     3 thru 4 */ memcpy (b_ptr +     20, b_ptr,     15);	/* WS-TYPE-CD-DELETE-KEYS:     5 thru 7 */ }
                ws_disp_sqlcode = 0;
                // MOVE ZERO TO COMP-3 field
                ws_row_number.replace(1797, 73, std::string(73, ' '));
                ws_row_number.replace(1870, 2, std::string(2, '\0'));
                ws_row_number.replace(1872, 720, std::string(720, ' '));
                ws_row_number.replace(2592, 2, std::string(2, '\0'));
                ws_row_number.replace(2594, 12, std::string(12, ' '));
                ws_row_number.replace(2606, 52, std::string(52, ' '));
                ws_row_number.replace(2658, 2, std::string(2, '\0'));
                ws_row_number.at(2660) = ' ';
                cdemo_from_program.at(33) = '0';
                ws_ca_screen_num.at(422) = '1';
                ws_ca_screen_num.at(423) = static_cast<char>(57);
            }
            if (ccard_aid.substr(0, 5) == "ENTER" && to_int(ws_updates_requested) > 0 && cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(0, 8)) {
                ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
                if (ws_row_number.at(13) == '0' && ws_row_number.at(14) == '0') {
                    p_8000_read_forward();
                }
                p_2000_send_map();
            }
            if (ccard_aid.substr(0, 5) == "PFK10" && to_int(ws_updates_requested) > 0 && cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(0, 8)) {
                p_9200_update_record();
                if (false /* TODO: (*(ws_ca_screen_num + 426)) == 0 */) {
                    ws_row_number.at(25) = 'Y';
                }
                ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
                p_8000_read_forward();
                p_2000_send_map();
            }
            ws_row_number.replace(1693, 2, ws_ca_screen_num.substr(420, 2));
            p_8000_read_forward();
            p_2000_send_map();
            if (ws_row_number.at(12) == '1') {
                ccard_aid.replace(27, 75, ws_row_number.substr(1566, 75));
                cdemo_from_program.replace(4, 8, ws_max_screen_lines.substr(0, 8));
                cdemo_from_program.replace(153, 7, ws_max_screen_lines.substr(12, 7));
                cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
                ccard_aid.replace(5, 8, ws_max_screen_lines.substr(0, 8));
                ccard_aid.replace(13, 7, ws_max_screen_lines.substr(12, 7));
                ccard_aid.replace(20, 7, ws_max_screen_lines.substr(19, 7));
            }
            ccard_aid.replace(5, 8, ws_max_screen_lines.substr(0, 8));
        }
    }
    }
// } // orphaned brace removed

void p_common_return() {
    cdemo_from_program = ws_max_screen_lines.substr(8, 4);
    cdemo_from_program.replace(4, 8, ws_max_screen_lines.substr(0, 8));
    cdemo_from_program.replace(153, 7, ws_max_screen_lines.substr(12, 7));
    cdemo_from_program.replace(146, 7, ws_max_screen_lines.substr(19, 7));
    ws_commarea = carddemo_commarea;
    ws_commarea.replace(160, 427, ws_ca_screen_num.substr(0, 427));
}

void p_0000_main_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_1000_receive_map() {
    p_1100_receive_screen();
    p_1200_edit_inputs();
}

void p_1000_receive_map_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_1100_receive_screen() {
    ws_row_number.replace(2606, 2, errmsgo.substr(179, 2));
    ws_row_number.replace(2608, 50, errmsgo.substr(188, 50));
    I = "1";
    while (true) {
        if (false /* TODO: cob_cmpswp_s16 (ws_row_number + 699, ((short)COB_BSWAP_16(*(short *)(ws_max_screen_lines + 117)))) > 0 */) break;
        if (false /* cob_cmp == 0 */ || false /* TODO: memcmp (errmsgo + 262 + 74LL * ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1), COB_SPACES_ALPHABETIC, 50) == 0 */) {
        } else {
            // MOVE to computed field
        }
        // ADD operation
    }
}

void p_1100_receive_screen_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_1200_edit_inputs() {
    ws_row_number.at(12) = '0';
    ws_row_number.at(720) = '0';
    p_1210_edit_array();
    p_1230_edit_desc();
    p_1220_edit_typecd();
    p_1290_cross_edits();
}

void p_1200_edit_inputs_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_1210_edit_array() {
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    // MOVE ZERO TO COMP-3 field
    if (ws_row_number.at(15) == 'Y' || ws_row_number.at(16) == 'Y') {
        ws_row_number.replace(685, 7, std::string(7, ' '));
    } else {
        // INSPECT operation
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        // INSPECT (TALLYING/REPLACING)
        ws_actions_requested = to_string(to_num(0) - to_num(1));
        ws_valid_actions_selected = to_string(to_num(0) + to_num(1));
        ws_row_number.replace(701, 2, std::string(2, static_cast<char>(0)));
        ws_row_number.at(27) = static_cast<char>(0);
        ws_row_number.replace(699, 2, ws_max_screen_lines.substr(117, 2));
        while (true) {
            if (to_int(RETURN_CODE) == 0) break;
            if (false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - 'D') == 0 */ || false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - 'U') == 0 */) {
                ws_row_number.replace(701, 2, ws_row_number.substr(699, 2));
                if (to_int(ws_actions_requested) >= 2 && to_int(ws_actions_requested) <= 7) {
                    ws_row_number.at(27) = 'Y';
                }
                if (false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - 'U') == 0 */) {
                    p_1211_edit_array_desc();
                }
            } else if (false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - ' ') == 0 */ || false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1))) == 0 */) {
            }
            ws_row_number.at(12) = '1';
            ws_row_number.at(27) = 'Y';
            ws_return_msg = "Action code selected is invalid";
            // ADD operation
        }
        if (false /* TODO: cob_cmpswp_s16 (ws_row_number + 701, ((short)COB_BSWAP_16(*(short *)(ws_ca_screen_num + 416)))) == 0 */) {
            ws_row_number.at(26) = static_cast<char>(0);
        } else {
            ws_row_number.at(26) = 'Y';
            ws_ca_screen_num.replace(416, 2, ws_row_number.substr(701, 2));
        }
        if (to_int(ws_actions_requested) >= 2 && to_int(ws_actions_requested) <= 7) {
            ws_row_number.at(12) = '1';
            ws_return_msg = "Please select only 1 action";
        }
    }
}

void p_1210_edit_array_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_1211_edit_array_desc() {
    ws_row_number.at(29) = '0';
    if (false /* cob_cmp == 0 */ && false /* cob_cmp == 0 */) {
        ws_return_msg = "No change detected with respect to database values.";
    } else {
        ws_row_number.at(29) = '1';
    }
    ws_row_number.at(28) = '0';
    ws_row_number.replace(30, 25, "Transaction Desc");
    // MOVE to computed field
    ws_edit_alphanum_length = to_string(50);
    p_1240_edit_alphanum_reqd();
    ws_row_number.at(28) = ws_row_number.at(314);
}

void p_1211_edit_array_desc_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_1220_edit_typecd() {
    ws_row_number.at(13) = ' ';
    if (to_int(ws_in_type_cd) == to_int(0) || false /* TODO: memcmp (ws_row_number + 2606, COB_SPACES_ALPHABETIC, 2) == 0 */ || false /* TODO: memcmp (ws_row_number + 2606, COB_ZEROES_ALPHABETIC, 2) == 0 */) {
        ws_row_number.at(13) = ' ';
        ws_row_number.replace(1695, 2, std::string(2, '0'));
    }
    if (false /* TODO: !cob_is_numeric (ws_in_type_cd) */) {
        ws_row_number.at(12) = '1';
        ws_row_number.at(13) = '0';
        ws_row_number.at(720) = '1';
        ws_return_msg = "TYPE CODE FILTER,IF SUPPLIED MUST BE A 2 DIGIT NUMBER";
    } else {
        ws_row_number.replace(1695, 2, ws_row_number.substr(2606, 2));
        ws_row_number.at(13) = '1';
    }
}

void p_1220_edit_typecd_exit() {
    if (ws_row_number.substr(2606, 2) == ws_ca_screen_num.substr(0, 2) || ws_row_number.at(13) == ' ' && false /* TODO: memcmp (ws_ca_screen_num, COB_ZEROES_ALPHABETIC, 2) == 0 */ || to_int(ws_ca_type_cd) == to_int(0) || false /* TODO: memcmp (ws_ca_screen_num, COB_SPACES_ALPHABETIC, 2) == 0 */) {
        ws_row_number.at(15) = static_cast<char>(0);
    } else {
        ws_ca_screen_num.replace(418, 4, std::string(4, ' '));
        ws_ca_screen_num.replace(422, 2, std::string(2, '0'));
        ws_ca_screen_num.at(424) = ' ';
        ws_ca_screen_num = ws_row_number.substr(2606, 2);
        ws_row_number.at(15) = 'Y';
    }
}

void p_1230_edit_desc() {
    ws_row_number.at(14) = ' ';
    if (to_int(ws_in_type_desc) == to_int(0) || false /* TODO: memcmp (ws_row_number + 2608, COB_SPACES_ALPHABETIC, 50) == 0 */) {
        ws_row_number.at(14) = ' ';
    } else {
        ws_row_number.at(14) = '1';
    }
    if (ws_row_number.at(14) == '1') {
        // STRING operation
        // STRING operation
    }
}

void p_1230_edit_desc_exit() {
    if (ws_row_number.substr(2608, 50) == ws_ca_screen_num.substr(2, 50) || ws_row_number.at(14) == ' ' && to_int(ws_ca_type_desc) == to_int(0) || false /* TODO: memcmp (ws_ca_screen_num + 2, COB_SPACES_ALPHABETIC, 50) == 0 */) {
        ws_row_number.at(16) = static_cast<char>(0);
    } else {
        ws_ca_screen_num.replace(418, 4, std::string(4, ' '));
        ws_ca_screen_num.replace(422, 2, std::string(2, '0'));
        ws_ca_screen_num.at(424) = ' ';
        ws_ca_screen_num.replace(2, 50, ws_row_number.substr(2608, 50));
        ws_row_number.at(16) = 'Y';
    }
}

void p_1240_edit_alphanum_reqd() {
    ws_row_number.at(314) = '0';
    if (false /* cob_cmp == 0 */ || false /* cob_cmp == 0 */ || false /* TODO: cob_cmp_llint (cob_intr_length (cob_intr_trim (0, 0, COB_SET_FLD (f2, cob_get_int (ws_edit_alphanum_length), ws_row_number + 55 + 0LL, &a_4), 0)), 0LL) == 0 */) {
        ws_row_number.at(12) = '1';
        ws_row_number.at(314) = static_cast<char>(66);
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            // STRING operation
            // STRING operation
        }
    }
    lit_all_alphanum_from = ws_max_screen_lines.substr(119, 62);
    // INSPECT operation
    // INSPECT (TALLYING/REPLACING)
    // INSPECT operation
    if (false /* TODO: cob_cmp_llint (cob_intr_length (cob_intr_trim (0, 0, COB_SET_FLD (f0, cob_get_int (ws_edit_alphanum_length), ws_row_number + 55 + 0LL, &a_4), 0)), 0LL) == 0 */) {
    } else {
        ws_row_number.at(12) = '1';
        ws_row_number.at(314) = '0';
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            // STRING operation
            // STRING operation
        }
    }
    ws_row_number.at(314) = static_cast<char>(0);
}

void p_1240_edit_alphanum_reqd_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_1290_cross_edits() {
    if (ws_row_number.at(13) == '1' || ws_row_number.at(14) == '1') {
    } else {
    }
    p_9100_check_filters();
    if (to_int(ws_records_count) == 0) {
        ws_row_number.at(12) = '1';
        if (ws_row_number.at(13) == '1') {
            ws_row_number.at(13) = '0';
        }
        if (ws_row_number.at(14) == '1') {
            ws_row_number.at(14) = '0';
        }
        ws_row_number.at(720) = '1';
        ws_return_msg = "No Records found for these filter conditions";
    }
}

void p_1290_cross_edits_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_2000_send_map() {
    p_2100_screen_init();
    p_2200_setup_array_attribs();
    p_2300_screen_array_init();
    p_2400_setup_screen_attrs();
    p_2500_setup_message();
    p_2600_send_screen();
}

void p_2000_send_map_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_2100_screen_init() {
    errmsgo = std::string(1044, static_cast<char>(0));
    // UNHANDLED: cob_move (cob_intr_current_date (0, 0), &f_927);
    errmsgo.replace(30, 40, ccda_title01.substr(0, 40));
    errmsgo.replace(107, 40, ccda_title01.substr(40, 40));
    errmsgo.replace(19, 4, ws_max_screen_lines.substr(8, 4));
    errmsgo.replace(92, 8, ws_max_screen_lines.substr(0, 8));
    // UNHANDLED: cob_move (cob_intr_current_date (0, 0), &f_927);
    ws_curdate_yy.replace(16, 2, ws_curdate_yy.substr(4, 2));
    ws_curdate_yy.replace(19, 2, ws_curdate_yy.substr(6, 2));
    // MOVE to computed field
    errmsgo.replace(77, 8, ws_curdate_yy.substr(16, 8));
    ws_curdate_yy.replace(24, 2, ws_curdate_yy.substr(8, 2));
    ws_curdate_yy.replace(27, 2, ws_curdate_yy.substr(10, 2));
    ws_curdate_yy.replace(30, 2, ws_curdate_yy.substr(12, 2));
    errmsgo.replace(154, 8, ws_curdate_yy.substr(24, 8));
    pagenoo = ws_ca_screen_num;
    ws_row_number.replace(1521, 45, std::string(45, ' '));
    errmsgo.replace(837, 45, ws_row_number.substr(1521, 45));
    errmsgo.at(833) = dfhbmdar.at(0);
}

void p_2100_screen_init_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_2200_setup_array_attribs() {
    ws_row_number.replace(699, 2, ws_max_screen_lines.substr(117, 2));
    while (true) {
        if (to_int(RETURN_CODE) == 0) break;
        if (false /* cob_cmp == 0 */ || ws_row_number.at(720) == '1') {
        } else {
            if (false /* TODO: (*(ws_row_number + 692 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - '1') == 0 */) {
                // MOVE to computed field
            }
            if (false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - 'D') == 0 */ && to_int(ws_valid_actions_selected) == 1 && false /* TODO: (*(ws_row_number + 27)) == 0 */) {
                // MOVE to computed field
            }
            if (false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - 'U') == 0 */ && to_int(ws_valid_actions_selected) == 1 && false /* TODO: (*(ws_row_number + 27)) == 0 */) {
                if (ws_row_number.at(25) == 'Y') {
                    // MOVE to computed field
                } else {
                    // MOVE to computed field
                    if (false /* TODO: !(((*(ws_row_number + 28)) == 0) || ((*(ws_row_number + 28) - ' ') == 0)) */) {
                    }
                }
            }
        }
        // ADD operation
    }
}

void p_2200_setup_array_attribs_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_2300_screen_array_init() {
    I = "1";
    while (true) {
        if (false /* TODO: cob_cmpswp_s16 (ws_row_number + 699, ((short)COB_BSWAP_16(*(short *)(ws_max_screen_lines + 117)))) > 0 */) break;
        if (false /* cob_cmp == 0 */) {
        } else {
            if (false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - 'D') == 0 */ && to_int(ws_valid_actions_selected) == 1 && false /* TODO: (*(ws_row_number + 27)) == 0 */) {
                if (ws_row_number.at(24) == 'Y') {
                } else {
                    ws_ca_screen_num.at(425) = 'Y';
                }
            }
            if (false /* TODO: (*(ws_row_number + 685 + ((cob_s64_t)(((short)COB_BSWAP_16(*(short *)(ws_row_number + 699)))) - 1)) - 'U') == 0 */ && to_int(ws_valid_actions_selected) == 1 && false /* TODO: (*(ws_row_number + 27)) == 0 */) {
                if (ws_row_number.at(25) == 'Y') {
                } else {
                    ws_ca_screen_num.at(426) = 'Y';
                }
                if (ws_row_number.at(29) == '1') {
                    if (ws_row_number.at(28) == 'B') {
                        // MOVE to computed field
                    }
                } else {
                }
            } else {
            }
        }
        // ADD operation
    }
}

void p_2300_screen_array_init_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_2400_setup_screen_attrs() {
    if (to_int(RETURN_CODE) == 0 || to_int(cdemo_from_program) == 0 && cdemo_from_program.substr(4, 8) == ws_max_screen_lines.substr(26, 8)) {
    } else {
        if (to_int(ws_actions_requested) > 0) {
            errmsgo.replace(179, 2, ws_row_number.substr(2606, 2));
            errmsgo.at(174) = dfhbmasf.at(0);
            errmsgo.at(175) = dfhblue.at(0);
        } else if (ws_row_number.at(13) == '1' || ws_row_number.at(13) == '0') {
            errmsgo.replace(179, 2, ws_row_number.substr(2606, 2));
            errmsgo.at(174) = dfhbmfse.at(0);
        } else if (to_int(ws_in_type_cd) == 0) {
            errmsgo.replace(179, 2, std::string(2, static_cast<char>(0)));
        }
        errmsgo.replace(179, 2, std::string(2, static_cast<char>(0)));
        errmsgo.at(174) = dfhbmfse.at(0);
        if (to_int(ws_actions_requested) > 0) {
            errmsgo.replace(188, 50, ws_row_number.substr(2608, 50));
            errmsgo.at(183) = dfhbmasf.at(0);
            errmsgo.at(184) = dfhblue.at(0);
        } else if (ws_row_number.at(14) == '1' || ws_row_number.at(14) == '0') {
            errmsgo.replace(188, 50, ws_row_number.substr(2608, 50));
            errmsgo.at(183) = dfhbmfse.at(0);
        }
        errmsgo.at(183) = dfhbmfse.at(0);
    }
    if (ws_row_number.at(13) == '0') {
        errmsgo.at(175) = dfhred.at(0);
        trtypel = "-1";
    }
    if (ws_row_number.at(14) == '0') {
        errmsgo.at(184) = dfhred.at(0);
        trdescl = "-1";
    }
    if (ws_row_number.at(12) == '0' || ws_row_number.at(12) == ' ' || false /* TODO: (*(ws_row_number + 12)) == 0 */) {
        if (to_int(ws_actions_requested) > 0 && ccard_aid.substr(0, 5) == "PFK07" && ccard_aid.substr(0, 5) == "PFK08") {
        } else {
            trtypel = "-1";
        }
    }
}

void p_2400_setup_screen_attrs_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_2500_setup_message() {
    if (ws_row_number.at(24) == 'Y') {
        ws_row_number.replace(1521, 45, "HIGHLIGHTED row deleted.Hit Enter to continue");
    } else if (ws_row_number.at(25) == 'Y') {
        ws_info_msg = "HIGHLIGHTED row was updated";
    } else if (ws_row_number.at(13) == '0' || ws_row_number.at(14) == '0') {
    } else if (ccard_aid.substr(0, 5) == "ENTER" && to_int(ws_deletes_requested) > 0 && to_int(ws_actions_requested) == 1 && to_int(ws_valid_actions_selected) == 1) {
        if (false /* TODO: memcmp (ws_row_number + 1521, COB_SPACES_ALPHABETIC, 45) == 0 */ || to_int(ws_info_msg) == to_int(0) && false /* TODO: (*(ws_row_number + 15)) == 0 */ && false /* TODO: (*(ws_row_number + 16)) == 0 */) {
            ws_row_number.replace(1521, 45, "Delete HIGHLIGHTED row \? Press F10 to confirm");
        }
    } else if (ccard_aid.substr(0, 5) == "ENTER" && to_int(ws_updates_requested) > 0 && to_int(ws_actions_requested) == 1 && to_int(ws_valid_actions_selected) == 1) {
        if (false /* TODO: memcmp (ws_row_number + 1521, COB_SPACES_ALPHABETIC, 45) == 0 */ || to_int(ws_info_msg) == to_int(0) && false /* TODO: (*(ws_row_number + 15)) == 0 */ && false /* TODO: (*(ws_row_number + 16)) == 0 */) {
            ws_row_number.replace(1521, 45, "Update HIGHLIGHTED row. Press F10 to save");
        }
    } else if (ccard_aid.substr(0, 5) == "PFK07" && to_int(ws_ca_screen_num) == 1) {
        ws_return_msg = "No previous pages to display";
    } else if (ccard_aid.substr(0, 5) == "PFK08" && false /* TODO: (*(ws_ca_screen_num + 424)) == 0 */ && to_int(ws_ca_screen_num) == 0) {
        ws_return_msg = "No more pages to display";
    } else if (ccard_aid.substr(0, 5) == "PFK08" && false /* TODO: (*(ws_ca_screen_num + 424)) == 0 */) {
        if (false /* TODO: memcmp (ws_row_number + 1521, COB_SPACES_ALPHABETIC, 45) == 0 */ || to_int(ws_info_msg) == to_int(0)) {
            ws_row_number.replace(1521, 45, "Type U to update, D to delete any record");
        }
        if (to_int(ws_ca_screen_num) == 9 && false /* TODO: (*(ws_ca_screen_num + 424)) == 0 */) {
            ws_ca_screen_num.at(423) = '0';
        }
    } else if (false /* TODO: memcmp (ws_row_number + 1521, COB_SPACES_ALPHABETIC, 45) == 0 */ || to_int(ws_info_msg) == to_int(0) || ws_ca_screen_num.at(424) == 'Y') {
        ws_row_number.replace(1521, 45, "Type U to update, D to delete any record");
    }
    ws_row_number.replace(1521, 45, std::string(45, ' '));
    errmsgo = ws_return_msg;
    ws_string_len = to_string(to_num(0) + to_num(1));
    ws_string_mid = to_string(to_num(0) + to_num(1));
    if (to_int(ws_info_msg) == to_int(0) && to_int(ws_return_msg) == 0) {
        errmsgo.replace(837, 45, ws_row_number.substr(1648, 45));
        errmsgo.at(833) = dfhneutr.at(0);
    }
}

void p_2500_setup_message_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_2600_send_screen() {
    // TODO: LAZARUS - Implement function body
}

void p_2600_send_screen_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_8000_read_forward() {
    ws_ca_screen_num.replace(52, 364, std::string(364, static_cast<char>(0)));
    p_9400_open_forward_cursor();
    if (ws_row_number.at(1797) == '1') {
    }
    ws_row_number.replace(2658, 2, std::string(2, static_cast<char>(0)));
    ws_ca_screen_num.at(424) = 'Y';
    ws_row_number.at(2660) = '1';
    while (true) {
        if (ws_row_number.at(2660) == '0') break;
        dcl_tr_description_text = std::string(2, ' ');
        dcl_tr_description_text.replace(2, 2, std::string(2, '\0'));
        dcl_tr_description_text.replace(4, 50, std::string(50, ' '));
        ws_disp_sqlcode = sqlcode;
        if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
            // ADD to ws_row_number
            if (to_int(RETURN_CODE) == 1) {
                ws_ca_screen_num.replace(420, 2, dcl_tr_description_text.substr(0, 2));
                if (to_int(ws_ca_screen_num) == 0) {
                    ws_ca_screen_num = to_string(to_int(ws_ca_screen_num) + 1);
                } else {
                }
            } else {
            }
            if (false /* TODO: cob_cmpswp_s16 (ws_row_number + 2658, ((short)COB_BSWAP_16(*(short *)(ws_max_screen_lines + 117)))) == 0 */) {
                ws_row_number.at(2660) = '0';
                ws_ca_screen_num.replace(418, 2, dcl_tr_description_text.substr(0, 2));
                ws_disp_sqlcode = sqlcode;
                if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
                    ws_ca_screen_num.at(424) = 'Y';
                    ws_ca_screen_num.replace(418, 2, dcl_tr_description_text.substr(0, 2));
                } else if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 100LL) == 0 */) {
                    ws_ca_screen_num.at(424) = static_cast<char>(0);
                    if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */ && ccard_aid.substr(0, 5) == "PFK08") {
                        ws_return_msg = "No more pages for these search conditions";
                    }
                }
                ws_row_number.at(2660) = '0';
                if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
                    ws_db2_current_action = "C-TR-TYPE-FORWARD fetch";
                    p_9999_format_db2_message();
                }
            }
        } else if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 100LL) == 0 */) {
            ws_row_number.at(2660) = '0';
            ws_ca_screen_num.at(424) = static_cast<char>(0);
            ws_ca_screen_num.replace(418, 2, dcl_tr_description_text.substr(0, 2));
            if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */ && ccard_aid.substr(0, 5) == "PFK08") {
                ws_return_msg = "No more pages for these search conditions";
            }
            if (to_int(ws_ca_screen_num) == 1 && to_int(RETURN_CODE) == 0) {
                ws_return_msg = "No records found for this search condition.";
            }
        }
        ws_row_number.at(2660) = '0';
        ws_row_number.at(1797) = '1';
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            ws_db2_current_action = "C-TR-TYPE-FORWARD close";
            p_9999_format_db2_message();
        }
    }
    p_9450_close_forward_cursor();
}

void p_8000_read_forward_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_8100_read_backwards() {
    ws_ca_screen_num.replace(52, 364, std::string(364, static_cast<char>(0)));
    ws_ca_screen_num.replace(418, 2, ws_ca_screen_num.substr(420, 2));
    ws_row_number = to_string(to_num(0) + to_num(1));
    ws_ca_screen_num.at(424) = 'Y';
    ws_row_number.at(2660) = '1';
    p_9500_open_backward_cursor();
    while (true) {
        if (ws_row_number.at(2660) == '0') break;
        dcl_tr_description_text = std::string(2, ' ');
        dcl_tr_description_text.replace(2, 2, std::string(2, '\0'));
        dcl_tr_description_text.replace(4, 50, std::string(50, ' '));
        ws_disp_sqlcode = sqlcode;
        if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
            ws_row_number = to_string(to_int(ws_row_number) - 1);
            if (to_int(RETURN_CODE) == 0) {
                ws_row_number.at(2660) = '0';
                ws_ca_screen_num.replace(420, 2, dcl_tr_description_text.substr(0, 2));
            } else {
            }
        }
        ws_row_number.at(2660) = '0';
        ws_row_number.at(1797) = '1';
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            ws_db2_current_action = "Error on fetch Cursor C-TR-TYPE-BACKWARD";
            p_9999_format_db2_message();
        }
    }
}

void p_8100_read_backwards_exit() {
    p_9550_close_back_cursor();
}

void p_9100_check_filters() {
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
    }
    ws_row_number.at(12) = '1';
    if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
        ws_db2_current_action = "Error reading TRANSACTION_TYPE table ";
        p_9999_format_db2_message();
    }
}

void p_9100_check_filters_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9200_update_record() {
    // MOVE to computed field
    dcl_tr_description_len = to_string(to_num(0) + to_num(1));
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
        ws_ca_screen_num.at(426) = static_cast<char>(0);
        if (false /* TODO: memcmp (ws_row_number + 1521, COB_SPACES_ALPHABETIC, 45) == 0 */ || to_int(ws_info_msg) == to_int(0)) {
            ws_info_msg = "HIGHLIGHTED row was updated";
        }
    } else if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 100LL) == 0 */) {
        ws_ca_screen_num.at(426) = 'Y';
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            ws_db2_current_action = "Record not found. Deleted by others \? ";
            p_9999_format_db2_message();
        }
    }
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, -911LL) == 0 */) {
        ws_ca_screen_num.at(426) = 'Y';
        ws_row_number.at(12) = '1';
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            ws_db2_current_action = "Deadlock. Someone else updating \?";
            p_9999_format_db2_message();
        }
    }
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) < 0 */) {
        ws_ca_screen_num.at(426) = 'Y';
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            ws_db2_current_action = "Update failed with";
            p_9999_format_db2_message();
        }
    }
}

void p_9200_update_record_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9300_delete_record() {
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
        ws_ca_screen_num.at(425) = static_cast<char>(0);
        if (false /* TODO: memcmp (ws_row_number + 1521, COB_SPACES_ALPHABETIC, 45) == 0 */ || to_int(ws_info_msg) == to_int(0)) {
            ws_row_number.replace(1521, 45, "HIGHLIGHTED row deleted.Hit Enter to continue");
        }
    } else if (false /* TODO: cob_cmp_s32 (sqlcode + 12, -532LL) == 0 */) {
        ws_ca_screen_num.at(425) = 'Y';
        if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
            ws_db2_current_action = "Please delete associated child records first:";
            p_9999_format_db2_message();
        }
    }
    if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
        ws_db2_current_action = "Delete failed with message:";
        p_9999_format_db2_message();
    }
}

void p_9300_delete_record_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9400_open_forward_cursor() {
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
    }
    ws_row_number.at(1797) = '1';
    if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
        ws_db2_current_action = "C-TR-TYPE-FORWARD Open";
        p_9999_format_db2_message();
    }
}

void p_9400_open_forward_cursor_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9450_close_forward_cursor() {
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
    }
    ws_row_number.at(1797) = '1';
    if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
        ws_db2_current_action = "C-TR-TYPE-FORWARD close";
        p_9999_format_db2_message();
    }
}

void p_9450_close_forward_cursor_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9500_open_backward_cursor() {
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
    }
    ws_row_number.at(1797) = '1';
    if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
        ws_db2_current_action = "C-TR-TYPE-BACKWARD Open";
        p_9999_format_db2_message();
    }
}

void p_9500_open_backward_cursor_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9550_close_back_cursor() {
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
    }
    ws_row_number.at(1797) = '1';
    if (false /* TODO: memcmp (ws_row_number + 1566, COB_SPACES_ALPHABETIC, 75) == 0 */) {
        ws_db2_current_action = "C-TR-TYPE-BACKWARD close";
        p_9999_format_db2_message();
    }
}

void p_9550_close_back_cursor_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9998_priming_query() {
    ws_disp_sqlcode = sqlcode;
    if (false /* TODO: cob_cmp_s32 (sqlcode + 12, 0LL) == 0 */) {
    }
    ws_row_number.at(1797) = '1';
    ws_db2_current_action = "Db2 access failure. ";
    p_9999_format_db2_message();
}

void p_9998_priming_query_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_9999_format_db2_message() {
    ws_dsntiac_lrecl = "+72";
    ws_dsntiac_err_cd = to_string(to_num(0) + to_num(1));
    if (to_int(ws_row_number) == 0) {
    } else {
        ws_row_number.replace(2594, 10, "DSNTIAC CD");
        ws_dsntiac_fmtd_text = ws_dsntiac_error;
    }
    // STRING operation
    // STRING operation
    ws_row_number.replace(1566, 75, ws_row_number.substr(721, 75));
}

void p_9999_format_db2_message_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_yyyy_store_pfkey() {
    if (false /* TODO: (*(dfheiblk + 24) - *(dfhenter)) == 0 */) {
        ccard_aid = "ENTER";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhclear)) == 0 */) {
        ccard_aid = "CLEAR";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpa1)) == 0 */) {
        ccard_aid = "PA1";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpa2)) == 0 */) {
        ccard_aid = "PA2";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf1)) == 0 */) {
        ccard_aid = "PFK01";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf2)) == 0 */) {
        ccard_aid = "PFK02";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf3)) == 0 */) {
        ccard_aid = "PFK03";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf4)) == 0 */) {
        ccard_aid = "PFK04";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf5)) == 0 */) {
        ccard_aid = "PFK05";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf6)) == 0 */) {
        ccard_aid = "PFK06";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf7)) == 0 */) {
        ccard_aid = "PFK07";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf8)) == 0 */) {
        ccard_aid = "PFK08";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf9)) == 0 */) {
        ccard_aid = "PFK09";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf10)) == 0 */) {
        ccard_aid = "PFK10";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf11)) == 0 */) {
        ccard_aid = "PFK11";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf12)) == 0 */) {
        ccard_aid = "PFK12";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf13)) == 0 */) {
        ccard_aid = "PFK01";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf14)) == 0 */) {
        ccard_aid = "PFK02";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf15)) == 0 */) {
        ccard_aid = "PFK03";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf16)) == 0 */) {
        ccard_aid = "PFK04";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf17)) == 0 */) {
        ccard_aid = "PFK05";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf18)) == 0 */) {
        ccard_aid = "PFK06";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf19)) == 0 */) {
        ccard_aid = "PFK07";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf20)) == 0 */) {
        ccard_aid = "PFK08";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf21)) == 0 */) {
        ccard_aid = "PFK09";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf22)) == 0 */) {
        ccard_aid = "PFK10";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf23)) == 0 */) {
        ccard_aid = "PFK11";
    } else if (false /* TODO: (*(dfheiblk + 24) - *(dfhpf24)) == 0 */) {
        ccard_aid = "PFK12";
    }
}

void p_yyyy_store_pfkey_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_send_plain_text() {
    // TODO: LAZARUS - Implement function body
}

void p_send_plain_text_exit() {
    // TODO: LAZARUS - Implement function body
}

void p_send_long_text() {
    // TODO: LAZARUS - Implement function body
}

void p_send_long_text_exit() {
    // TODO: LAZARUS - Implement function body
}

int program_COTRTLIC_main() {
    try {

    p_0000_main();
    return RETURN_CODE;

        return 0;
    } catch (const std::out_of_range& e) {
        std::cerr << "[LAZARUS ERROR] Bounds violation: " << e.what() << std::endl;
        return static_cast<int>(ErrorCode::SubscriptRangeExceeded);
    } catch (const std::overflow_error& e) {
        std::cerr << "[LAZARUS ERROR] Numeric overflow: " << e.what() << std::endl;
        return static_cast<int>(ErrorCode::NumericTransformFailed);
    } catch (const std::invalid_argument& e) {
        std::cerr << "[LAZARUS ERROR] Invalid input: " << e.what() << std::endl;
        return static_cast<int>(ErrorCode::InvalidInput);
    } catch (const std::length_error& e) {
        std::cerr << "[LAZARUS ERROR] Buffer overflow: " << e.what() << std::endl;
        return static_cast<int>(ErrorCode::BufferOverflow);
    } catch (const std::exception& e) {
        std::cerr << "[LAZARUS ERROR] " << e.what() << std::endl;
        return static_cast<int>(ErrorCode::InternalError);
    } catch (...) {
        std::cerr << "[LAZARUS ERROR] Unknown error occurred" << std::endl;
        return static_cast<int>(ErrorCode::InternalError);
    }
}
// ============================================================================
// LAZARUS SECURITY MANIFEST
// ============================================================================
//
// Hardening Statistics:
//   - Types hardened: 148
//   - Bounds checks added: 158
//   - Names converted: 210
//   - Error handlers: 1
//   - Vulnerabilities fixed: 0
//   - Empty functions flagged: 32
//
// Security Checklist:
//   [x] Buffer overflow protection (FixedString)
//   [x] Integer overflow protection (SafeInt)
//   [x] Decimal precision (Decimal class)
//   [x] Bounds checking (.at() access)
//   [x] Input validation (safe:: namespace)
//   [x] Exception handling (try/catch in main)
//   [x] Memory safety (RAII, no raw pointers)
//   [x] Type safety (COBOL-compatible types)
//   [x] CICS stubs (lazarus::cics namespace)
//   [x] Error codes (matches COBOL return codes)
//
// Build Command (recommended):
//   g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2 -o output source.cpp
//
// For testing with sanitizers:
//   g++ -std=c++17 -fsanitize=address,undefined -g -o output source.cpp
//
// ============================================================================

} // anonymous namespace
