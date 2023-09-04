#include <stdio.h>
#include <math.h>

unsigned int Ivcp_Ue(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit)
{
    // 计算0bit的个数
    unsigned int nZeroNum = 0;
    while (nStartBit < nLen * 8)
    {
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) //&:按位与，%取余
        {
            break;
        }
        nZeroNum++;
        nStartBit++;
    }
    nStartBit++;

    // 计算结果
    unsigned long dwRet = 0;
    for (unsigned int i = 0; i < nZeroNum; i++)
    {
        dwRet <<= 1;
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
        {
            dwRet += 1;
        }
        nStartBit++;
    }
    return (1 << nZeroNum) - 1 + dwRet;
}

int Ivcp_Se(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit)
{
    int UeVal = Ivcp_Ue(pBuff, nLen, nStartBit);
    double k = UeVal;
    int nValue = ceil(k / 2); // ceil函数：ceil函数的作用是求不小于给定实数的最小整数。ceil(2)=ceil(1.2)=cei(1.5)=2.00
    if (UeVal % 2 == 0)
        nValue = -nValue;
    return nValue;
}

unsigned long Ivcp_u(unsigned int BitCount, unsigned char *buf, unsigned int &nStartBit)
{
    unsigned long dwRet = 0;
    for (unsigned int i = 0; i < BitCount; i++)
    {
        dwRet <<= 1;
        if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
        {
            dwRet += 1;
        }
        nStartBit++;
    }
    return dwRet;
}

void Ivcp_de_emulation_prevention(unsigned char *buf, unsigned int *buf_size)
{
    int i = 0, j = 0;
    unsigned char *tmp_ptr = NULL;
    unsigned int tmp_buf_size = 0;
    int val = 0;

    tmp_ptr = buf;
    tmp_buf_size = *buf_size;
    for (i = 0; i < (tmp_buf_size - 2); i++)
    {
        // check for 0x000003
        val = (tmp_ptr[i] ^ 0x00) + (tmp_ptr[i + 1] ^ 0x00) + (tmp_ptr[i + 2] ^ 0x03);
        if (val == 0)
        {
            // kick out 0x03
            for (j = i + 2; j < tmp_buf_size - 1; j++)
                tmp_ptr[j] = tmp_ptr[j + 1];

            // and so we should devrease bufsize
            (*buf_size)--;
        }
    }

    return;
}

bool Ivcp_h264_decode_sps(unsigned char *buf, unsigned int nLen, int &width, int &height, int &fps)
{
    unsigned int StartBit = 0;
    Ivcp_de_emulation_prevention(buf, &nLen);
    int forbidden_zero_bit = Ivcp_u(1, buf, StartBit);
    int nal_ref_idc = Ivcp_u(2, buf, StartBit);
    int nal_unit_type = Ivcp_u(5, buf, StartBit);
    if (nal_unit_type == 7)
    {
        int profile_idc = Ivcp_u(8, buf, StartBit);
        int constraint_set0_flag = Ivcp_u(1, buf, StartBit); //(buf[1] & 0x80)>>7;
        int constraint_set1_flag = Ivcp_u(1, buf, StartBit); //(buf[1] & 0x40)>>6;
        int constraint_set2_flag = Ivcp_u(1, buf, StartBit); //(buf[1] & 0x20)>>5;
        int constraint_set3_flag = Ivcp_u(1, buf, StartBit); //(buf[1] & 0x10)>>4;
        int reserved_zero_4bits = Ivcp_u(4, buf, StartBit);
        int level_idc = Ivcp_u(8, buf, StartBit);

        int seq_parameter_set_id = Ivcp_Ue(buf, nLen, StartBit);

        if (profile_idc == 100 || profile_idc == 110 ||
            profile_idc == 122 || profile_idc == 144)
        {
            int chroma_format_idc = Ivcp_Ue(buf, nLen, StartBit);
            if (chroma_format_idc == 3)
                int residual_colour_transform_flag = Ivcp_u(1, buf, StartBit);
            int bit_depth_luma_minus8 = Ivcp_Ue(buf, nLen, StartBit);
            int bit_depth_chroma_minus8 = Ivcp_Ue(buf, nLen, StartBit);
            int qpprime_y_zero_transform_bypass_flag = Ivcp_u(1, buf, StartBit);
            int seq_scaling_matrix_present_flag = Ivcp_u(1, buf, StartBit);

            int seq_scaling_list_present_flag[8];
            if (seq_scaling_matrix_present_flag)
            {
                for (int i = 0; i < 8; i++)
                {
                    seq_scaling_list_present_flag[i] = Ivcp_u(1, buf, StartBit);
                }
            }
        }
        int log2_max_frame_num_minus4 = Ivcp_Ue(buf, nLen, StartBit);
        int pic_order_cnt_type = Ivcp_Ue(buf, nLen, StartBit);
        if (pic_order_cnt_type == 0)
            int log2_max_pic_order_cnt_lsb_minus4 = Ivcp_Ue(buf, nLen, StartBit);
        else if (pic_order_cnt_type == 1)
        {
            int delta_pic_order_always_zero_flag = Ivcp_u(1, buf, StartBit);
            int offset_for_non_ref_pic = Ivcp_Se(buf, nLen, StartBit);
            int offset_for_top_to_bottom_field = Ivcp_Se(buf, nLen, StartBit);
            int num_ref_frames_in_pic_order_cnt_cycle = Ivcp_Ue(buf, nLen, StartBit);

            int *offset_for_ref_frame = new int[num_ref_frames_in_pic_order_cnt_cycle];
            for (int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
                offset_for_ref_frame[i] = Ivcp_Se(buf, nLen, StartBit);
            delete[] offset_for_ref_frame;
        }
        int num_ref_frames = Ivcp_Ue(buf, nLen, StartBit);
        int gaps_in_frame_num_value_allowed_flag = Ivcp_u(1, buf, StartBit);
        int pic_width_in_mbs_minus1 = Ivcp_Ue(buf, nLen, StartBit);
        int pic_height_in_map_units_minus1 = Ivcp_Ue(buf, nLen, StartBit);

        width = (pic_width_in_mbs_minus1 + 1) * 16;
        height = (pic_height_in_map_units_minus1 + 1) * 16;

        int frame_mbs_only_flag = Ivcp_u(1, buf, StartBit);
        if (!frame_mbs_only_flag)
            int mb_adaptive_frame_field_flag = Ivcp_u(1, buf, StartBit);

        int direct_8x8_inference_flag = Ivcp_u(1, buf, StartBit);
        int frame_cropping_flag = Ivcp_u(1, buf, StartBit);
        if (frame_cropping_flag)
        {
            int frame_crop_left_offset = Ivcp_Ue(buf, nLen, StartBit);
            int frame_crop_right_offset = Ivcp_Ue(buf, nLen, StartBit);
            int frame_crop_top_offset = Ivcp_Ue(buf, nLen, StartBit);
            int frame_crop_bottom_offset = Ivcp_Ue(buf, nLen, StartBit);
        }
        int vui_parameter_present_flag = Ivcp_u(1, buf, StartBit);
        if (vui_parameter_present_flag)
        {
            int aspect_ratio_info_present_flag = Ivcp_u(1, buf, StartBit);
            if (aspect_ratio_info_present_flag)
            {
                int aspect_ratio_idc = Ivcp_u(8, buf, StartBit);
                if (aspect_ratio_idc == 255)
                {
                    int sar_width = Ivcp_u(16, buf, StartBit);
                    int sar_height = Ivcp_u(16, buf, StartBit);
                }
            }
            int overscan_info_present_flag = Ivcp_u(1, buf, StartBit);
            if (overscan_info_present_flag)
                int overscan_appropriate_flagu = Ivcp_u(1, buf, StartBit);
            int video_signal_type_present_flag = Ivcp_u(1, buf, StartBit);
            if (video_signal_type_present_flag)
            {
                int video_format = Ivcp_u(3, buf, StartBit);
                int video_full_range_flag = Ivcp_u(1, buf, StartBit);
                int colour_description_present_flag = Ivcp_u(1, buf, StartBit);
                if (colour_description_present_flag)
                {
                    int colour_primaries = Ivcp_u(8, buf, StartBit);
                    int transfer_characteristics = Ivcp_u(8, buf, StartBit);
                    int matrix_coefficients = Ivcp_u(8, buf, StartBit);
                }
            }
            int chroma_loc_info_present_flag = Ivcp_u(1, buf, StartBit);
            if (chroma_loc_info_present_flag)
            {
                int chroma_sample_loc_type_top_field = Ivcp_Ue(buf, nLen, StartBit);
                int chroma_sample_loc_type_bottom_field = Ivcp_Ue(buf, nLen, StartBit);
            }
            int timing_info_present_flag = Ivcp_u(1, buf, StartBit);
            if (timing_info_present_flag)
            {
                int num_units_in_tick = Ivcp_u(32, buf, StartBit);
                int time_scale = Ivcp_u(32, buf, StartBit);
                fps = time_scale / (2 * num_units_in_tick);
            }
        }

        return true;
    }
    else
        return false;
}