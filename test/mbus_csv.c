//------------------------------------------------------------------------------
// Copyright (C) 2012, Alexander Rössler
// All rights reserved.
//
//
//------------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
//#include <bits/mathcalls.h>

#include "mbus_csv.h"

int debug = 0;

//------------------------------------------------------------------------------
///
/// Lookup the unit description from a VIF field in a data record
///
//------------------------------------------------------------------------------
const char *
mbus_unit_prefix2(int *exp)
{
    static char buff[256];
    
    switch (*exp)
    {
        case 0:
            buff[0] = 0;
            break;

        case -3:
            snprintf(buff, sizeof(buff), "m");
	    *exp = 0;
            break;
       
        case -6:
            snprintf(buff, sizeof(buff), "my");
	    *exp = 0;
            break;

        case 3:
            snprintf(buff, sizeof(buff), "k");
	    *exp = 0;
            break;
            
        case 4:
            snprintf(buff, sizeof(buff), "k");
	    *exp = 1;
            break;            

        case 5:
            snprintf(buff, sizeof(buff), "k");
	    *exp = 2;
            break;            

        case 6:
            snprintf(buff, sizeof(buff), "M");
	    *exp = 0;
            break;
       
        case 9:
            snprintf(buff, sizeof(buff), "T");
	    *exp = 0;
            break;
           
        default:
	  buff[0] = 0;
            //snprintf(buff, sizeof(buff), "1e%d ", *exp);
    }
        
    return buff;
}

//------------------------------------------------------------------------------
/// Look up the unit from a VIF field in the data record.
/// 
/// See section 8.4.3  Codes for Value Information Field (VIF) in the M-BUS spec
//------------------------------------------------------------------------------
const char *
mbus_vif_unit_lookup2(u_char vif, int *prefix)
{
    static char buff[256];
    int n = 0;

    switch (vif & 0x7F) // ignore the extension bit in this selection
    {
        // E000 0nnn Energy 10(nnn-3) W
        case 0x00:
        case 0x00+1:
        case 0x00+2:
        case 0x00+3:
        case 0x00+4:
        case 0x00+5:
        case 0x00+6:
        case 0x00+7:
            n = (vif & 0x07) - 3;
            snprintf(buff, sizeof(buff), "%sWh", mbus_unit_prefix2(&n)); 
            break;

        // 0000 1nnn          Energy       10(nnn)J     (0.001kJ to 10000kJ)
        case 0x08:
        case 0x08+1:
        case 0x08+2:
        case 0x08+3:
        case 0x08+4:
        case 0x08+5:
        case 0x08+6:
        case 0x08+7:
            
            n = (vif & 0x07);
            snprintf(buff, sizeof(buff), "%sJ", mbus_unit_prefix2(&n));
  
            break;        

        // E001 1nnn Mass 10(nnn-3) kg 0.001kg to 10000kg
        case 0x18:
        case 0x18+1:
        case 0x18+2:
        case 0x18+3:
        case 0x18+4:
        case 0x18+5:
        case 0x18+6:
        case 0x18+7:
            
            n = (vif & 0x07);
	    n = n-3;
            snprintf(buff, sizeof(buff), "%skg", mbus_unit_prefix2(&n));
  
            break;        

        // E010 1nnn Power 10(nnn-3) W 0.001W to 10000W
        case 0x28:
        case 0x28+1:
        case 0x28+2:
        case 0x28+3:
        case 0x28+4:
        case 0x28+5:
        case 0x28+6:
        case 0x28+7:

            n = (vif & 0x07);
	    n = n -3;
            snprintf(buff, sizeof(buff), "%sW", mbus_unit_prefix2(&n));
            //snprintf(buff, sizeof(buff), "Power (10^%d W)", n-3);
  
            break;
            
        // E011 0nnn Power 10(nnn) J/h 0.001kJ/h to 10000kJ/h
        case 0x30:
        case 0x30+1:
        case 0x30+2:
        case 0x30+3:
        case 0x30+4:
        case 0x30+5:
        case 0x30+6:
        case 0x30+7:

            n = (vif & 0x07);
            snprintf(buff, sizeof(buff), "%sJ/h", mbus_unit_prefix2(&n));
  
            break;       

        // E001 0nnn Volume 10(nnn-6) m3 0.001l to 10000l
        case 0x10:
        case 0x10+1:
        case 0x10+2:
        case 0x10+3:
        case 0x10+4:
        case 0x10+5:
        case 0x10+6:
        case 0x10+7:

            n = (vif & 0x07);
	    n = n-6;
            snprintf(buff, sizeof(buff), "%sm³", mbus_unit_prefix2(&n));
  
            break;
        
        // E011 1nnn Volume Flow 10(nnn-6) m3/h 0.001l/h to 10000l/
        case 0x38:
        case 0x38+1:
        case 0x38+2:
        case 0x38+3:
        case 0x38+4:
        case 0x38+5:
        case 0x38+6:
        case 0x38+7:

            n = (vif & 0x07);
	    n = n-6;
            snprintf(buff, sizeof(buff), "%sm³/h", mbus_unit_prefix2(&n));
  
            break;     

        // E100 0nnn Volume Flow ext. 10(nnn-7) m3/min 0.0001l/min to 1000l/min
        case 0x40:
        case 0x40+1:
        case 0x40+2:
        case 0x40+3:
        case 0x40+4:
        case 0x40+5:
        case 0x40+6:
        case 0x40+7:

            n = (vif & 0x07);
	    n = n-7;
            snprintf(buff, sizeof(buff), "%sm³/min", mbus_unit_prefix2(&n));
  
            break;  

        // E100 1nnn Volume Flow ext. 10(nnn-9) m3/s 0.001ml/s to 10000ml/
        case 0x48:
        case 0x48+1:
        case 0x48+2:
        case 0x48+3:
        case 0x48+4:
        case 0x48+5:
        case 0x48+6:
        case 0x48+7:

            n = (vif & 0x07);
	    n = n-9;
            snprintf(buff, sizeof(buff), "%sm³/s", mbus_unit_prefix2(&n));
  
            break;       

        // E101 0nnn Mass flow 10(nnn-3) kg/h 0.001kg/h to 10000kg/
        case 0x50:
        case 0x50+1:
        case 0x50+2:
        case 0x50+3:
        case 0x50+4:
        case 0x50+5:
        case 0x50+6:
        case 0x50+7:

            n = (vif & 0x07);
	    n = n-3;
            snprintf(buff, sizeof(buff), "%skg/h", mbus_unit_prefix2(&n));
  
            break;       

        // E101 10nn Flow Temperature 10(nn-3) °C 0.001°C to 1°C
        case 0x58:
        case 0x58+1:
        case 0x58+2:
        case 0x58+3:

            n = (vif & 0x03);
	    n = n-3;
            snprintf(buff, sizeof(buff), "%s°C", mbus_unit_prefix2(&n));
  
            break;

        // E101 11nn Return Temperature 10(nn-3) °C 0.001°C to 1°C
        case 0x5C:
        case 0x5C+1:
        case 0x5C+2:
        case 0x5C+3:

            n = (vif & 0x03);
	    n = n-3;
            snprintf(buff, sizeof(buff), "%s°C", mbus_unit_prefix2(&n));
  
            break;

        // E110 10nn Pressure 10(nn-3) bar 1mbar to 1000mbar
        case 0x68:
        case 0x68+1:
        case 0x68+2:
        case 0x68+3:

            n = (vif & 0x03);
	    n = n-3;
            snprintf(buff, sizeof(buff), "%sbar", mbus_unit_prefix2(&n));
  
            break;

        // E010 00nn On Time
        // nn = 00 seconds
        // nn = 01 minutes
        // nn = 10   hours
        // nn = 11    days
        // E010 01nn Operating Time coded like OnTime
        // E111 00nn Averaging Duration	coded like OnTime	 
        // E111 01nn Actuality Duration coded like OnTime
        case 0x20:
        case 0x20+1:
        case 0x20+2:
        case 0x20+3:
        case 0x24:
        case 0x24+1:
        case 0x24+2:
        case 0x24+3:
        case 0x70:
        case 0x70+1:
        case 0x70+2:
        case 0x70+3:
        case 0x74:
        case 0x74+1:
        case 0x74+2:
        case 0x74+3:
            {
                int offset = 0;

                /*if      ((vif & 0x7C) == 0x20)
                    offset = snprintf(buff, sizeof(buff), "On time ");
                else if ((vif & 0x7C) == 0x24) 
                    offset = snprintf(buff, sizeof(buff), "Operating time ");
                else if ((vif & 0x7C) == 0x70)
                    offset = snprintf(buff, sizeof(buff), "Averaging Duration ");
                else
                    offset = snprintf(buff, sizeof(buff), "Actuality Duration ");*/
                
                switch (vif & 0x03)
                {
                    case 0x00:
                        snprintf(&buff[offset], sizeof(buff)-offset, "seconds");
                        break;
                    case 0x01:
                        snprintf(&buff[offset], sizeof(buff)-offset, "minutes");
                        break;
                    case 0x02:
                        snprintf(&buff[offset], sizeof(buff)-offset, "hours");
                        break;
                    case 0x03:
                        snprintf(&buff[offset], sizeof(buff)-offset, "days");
                        break;
                }
            }
            break;                     

        // E110 110n Time Point
        // n = 0        date
        // n = 1 time & date
        // data type G
        // data type F
        case 0x6C:
        case 0x6C+1:

            if (vif & 0x1)
                snprintf(buff, sizeof(buff), "Time Point (time & date)");
            else
                snprintf(buff, sizeof(buff), "Time Point (date)");
  
            break;
            
        // E110 00nn    Temperature Difference   10(nn-3)K   (mK to  K)
        case 0x60:
        case 0x60+1:
        case 0x60+2:
        case 0x60+3:

            n = (vif & 0x03);
            n = n-3;
            snprintf(buff, sizeof(buff), "%s°C", mbus_unit_prefix2(&n));
                        
            break;

        // E110 01nn External Temperature 10(nn-3) °C 0.001°C to 1°C
        case 0x64:
        case 0x64+1:
        case 0x64+2:
        case 0x64+3:

            n = (vif & 0x03);
	    n = n-3;
            snprintf(buff, sizeof(buff), "°C", mbus_unit_prefix2(&n));
  
            break;

        // E110 1110 Units for H.C.A. dimensionless
        case 0x6E:
            snprintf(buff, sizeof(buff), "Units for H.C.A.");
            break; 

        // E110 1111 Reserved
        case 0x6F:
            snprintf(buff, sizeof(buff), "Reserved");
            break;        

        // Custom VIF in the following string: never reached...
        case 0x7C:
            snprintf(buff, sizeof(buff), "Custom VIF");
            break;        

        // Fabrication No
        case 0x78:
            snprintf(buff, sizeof(buff), "Fabrication number");
            break;
            
        // Bus Address
        case 0x7A:
            snprintf(buff, sizeof(buff), "Bus Address");
            break;

        // Manufacturer specific: 7Fh / FF  
        case 0x7F:
        case 0xFF:
            snprintf(buff, sizeof(buff), "Manufacturer specific");
            break;
                       
        default:
        
            snprintf(buff, sizeof(buff), "Unknown (VIF=0x%.2X)", vif);
            break;
    }
    
    if (prefix != NULL)
      *prefix = n;
    return buff;
}

//------------------------------------------------------------------------------
/// Lookup the unit from the VIB (VIF or VIFE)
//
//  Enhanced Identification
//    E000 1000      Access Number (transmission count) 
//    E000 1001      Medium (as in fixed header) 
//    E000 1010      Manufacturer (as in fixed header) 
//    E000 1011      Parameter set identification 
//    E000 1100      Model / Version 
//    E000 1101      Hardware version # 
//    E000 1110      Firmware version # 
//    E000 1111      Software version # 
//------------------------------------------------------------------------------
const char *
mbus_vib_unit_lookup2(mbus_value_information_block *vib, int *prefix)
{   
    static char buff[256];
    int n;

    if (vib->vif == 0xFD || vib->vif == 0xFB) // first type of VIF extention: see table 8.4.4 
    {
        if (vib->nvife == 0)
        {
            snprintf(buff, sizeof(buff), "Missing VIF extension");
        }
        else if (vib->vife[0] == 0x08 || vib->vife[0] == 0x88)
        {
            // E000 1000 
            snprintf(buff, sizeof(buff), "Access Number (transmission count)");
        }
        else if (vib->vife[0] == 0x09|| vib->vife[0] == 0x89)
        {
            // E000 1001
            snprintf(buff, sizeof(buff), "Medium (as in fixed header)");
        }
        else if (vib->vife[0] == 0x0A || vib->vife[0] == 0x8A)
        {
            // E000 1010
            snprintf(buff, sizeof(buff), "Manufacturer (as in fixed header)");
        }
        else if (vib->vife[0] == 0x0B || vib->vife[0] == 0x8B)
        {
            // E000 1010
            snprintf(buff, sizeof(buff), "Parameter set identification");
        }
        else if (vib->vife[0] == 0x0C || vib->vife[0] == 0x8C)
        {
            // E000 1100
            snprintf(buff, sizeof(buff), "Model / Version");
        }
        else if (vib->vife[0] == 0x0D || vib->vife[0] == 0x8D)
        {
            // E000 1100
            snprintf(buff, sizeof(buff), "Hardware version");
        }
        else if (vib->vife[0] == 0x0E || vib->vife[0] == 0x8E)
        {
            // E000 1101
            snprintf(buff, sizeof(buff), "Firmware version");
        }
        else if (vib->vife[0] == 0x0F || vib->vife[0] == 0x8F)
        {
            // E000 1101
            snprintf(buff, sizeof(buff), "Software version");
        }
        else if (vib->vife[0] == 0x17 || vib->vife[0] == 0x97)
		{
            // VIFE = E001 0111 Error flags
            snprintf(buff, sizeof(buff), "Error flags");
		}
        else if (vib->vife[0] == 0x10)
        {
            // VIFE = E001 0000 Customer location
            snprintf(buff, sizeof(buff), "Customer location");
        }
        else if (vib->vife[0] == 0x0C)
        {
            // E000 1100 Model / Version
            snprintf(buff, sizeof(buff), "Model / Version");
        }
        else if (vib->vife[0] == 0x11)
        {
            // VIFE = E001 0001 Customer
            snprintf(buff, sizeof(buff), "Customer");
        }
        else if (vib->vife[0] == 0x9)
        {
            // VIFE = E001 0110 Password
            snprintf(buff, sizeof(buff), "Password");
        }
        else if (vib->vife[0] == 0x0b)
        {
            // VIFE = E000 1011 Parameter set identification
            snprintf(buff, sizeof(buff), "Parameter set identification");
        }
        else if ((vib->vife[0] & 0x70) == 0x40)
        {
            // VIFE = E100 nnnn 10^(nnnn-9) V
            n = (vib->vife[0] & 0x0F);
            snprintf(buff, sizeof(buff), "%s V", mbus_unit_prefix(n-9));
        }
        else if ((vib->vife[0] & 0x70) == 0x50)
        {
            // VIFE = E101 nnnn 10nnnn-12 A
            n = (vib->vife[0] & 0x0F);
            snprintf(buff, sizeof(buff), "%s A", mbus_unit_prefix(n-12));
        }
        else if ((vib->vife[0] & 0xF0) == 0x70)
        {
            // VIFE = E111 nnn Reserved
            snprintf(buff, sizeof(buff), "Reserved VIF extension");
        }
        else
        {
            snprintf(buff, sizeof(buff), "Unrecongized VIF extension: 0x%.2x", vib->vife[0]);
        }
        return buff;
    }
    else if (vib->vif == 0x7C)
    {
        // custom VIF
        snprintf(buff, sizeof(buff), "%s", vib->custom_vif);
        return buff;
    }

    return mbus_vif_unit_lookup2(vib->vif, prefix); // no extention, use VIF
}

//------------------------------------------------------------------------------
// Decode data and write to string
//
// Data format (for record->data data array)
// 
// Length in Bit   Code    Meaning           Code      Meaning
//      0          0000    No data           1000      Selection for Readout
//      8          0001     8 Bit Integer    1001      2 digit BCD
//     16          0010    16 Bit Integer    1010      4 digit BCD
//     24          0011    24 Bit Integer    1011      6 digit BCD
//     32          0100    32 Bit Integer    1100      8 digit BCD
//   32 / N        0101    32 Bit Real       1101      variable length
//     48          0110    48 Bit Integer    1110      12 digit BCD
//     64          0111    64 Bit Integer    1111      Special Functions
//
// The Code is stored in record->drh.dib.dif
//
///
/// Return a string containing the data
/// dataType: 0 = number -> value, 1 = string
///
// Source: MBDOC48.PDF
//
//------------------------------------------------------------------------------
const char *
mbus_data_record_decode2(mbus_data_record *record, int *dataType, double *value)
{
    static char buff[256];
    u_char vif, vife;
    
    // ignore extension bit
    vif = (record->drh.vib.vif & 0x7F);       
    vife = (record->drh.vib.vife[0] & 0x7F);
    
    if (dataType != NULL)	//define default data type string
      *dataType = 1;

    if (record)
    {
        int val;
        long val2;
	float val3;
        struct tm time;
            
        switch (record->drh.dib.dif & 0x0F)
        {
            case 0x00: // no data
        
                buff[0] = 0;
		
                break; 

            case 0x01: // 1 byte integer (8 bit)
        
                val = mbus_data_int_decode(record->data, 1);
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val;
               
                snprintf(buff, sizeof(buff), "%d", val);

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 1 byte integer\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break; 


            case 0x02: // 2 byte (16 bit)
                
                // E110 1100  Time Point (date)
                if (vif == 0x6C)            
                {
                    mbus_data_tm_decode(&time, record->data, 2);
                    snprintf(buff, sizeof(buff), "%04d-%02d-%02d", 
                                                 (time.tm_year + 2000), 
                                                 (time.tm_mon + 1), 
                                                  time.tm_mday);
                }
                else  // 2 byte integer
                {
                    val = mbus_data_int_decode(record->data, 2);
		    if (dataType != NULL)
		      *dataType = 0;
		    if (value != NULL)
		      *value = (double)val;
		    
                    snprintf(buff, sizeof(buff), "%d", val);
                    if (debug)
                        printf("%s: DIF 0x%.2x was decoded using 2 byte integer\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                }
        
                break; 

            case 0x03: // 3 byte integer (24 bit)

                val = mbus_data_int_decode(record->data, 3);
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val;
                        
                snprintf(buff, sizeof(buff), "%d", val);

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 3 byte integer\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break; 
                
            case 0x04: // 4 byte (32 bit)
            
                // E110 1101  Time Point (date/time)
                // E011 0000  Start (date/time) of tariff
                // E111 0000  Date and time of battery change
                if ( (vif == 0x6D) ||                                     
                    ((record->drh.vib.vif == 0xFD) && (vife == 0x30)) ||  
                    ((record->drh.vib.vif == 0xFD) && (vife == 0x70)))    
                {
                    mbus_data_tm_decode(&time, record->data, 4);
                    snprintf(buff, sizeof(buff), "%04d-%02d-%02dT%02d:%02d:%02d", 
                                                 (time.tm_year + 2000), 
                                                 (time.tm_mon + 1), 
                                                  time.tm_mday,
                                                  time.tm_hour,
                                                  time.tm_min,
                                                  time.tm_sec);
                }
                else  // 4 byte integer
                {
                    val = mbus_data_int_decode(record->data, 4);
		    if (dataType != NULL)
		      *dataType = 0;
		    if (value != NULL)
		      *value = (double)val;
                    snprintf(buff, sizeof(buff), "%d", val);
                }

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 4 byte integer\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;  

	    case 0x05: // 4 byte float (32 bit)
	      
		val3 = mbus_data_float_decode(record->data, 4);
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val3;
		
		snprintf(buff, sizeof(buff), "%f", val3);
		
		if (debug)
		  printf("%s: DIF 0x%.2x was decoded using 4 byte float\n", __PRETTY_FUNCTION__, record->drh.dib.dif);
		
		break;

            case 0x06: // 6 byte integer (48 bit)

                val2 = mbus_data_long_decode(record->data, 6);
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val2;
        
                snprintf(buff, sizeof(buff), "%lu", val2);

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 6 byte integer\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;          

            case 0x07: // 8 byte integer (64 bit)

                val2 = mbus_data_long_decode(record->data, 8);
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val2;
        
                snprintf(buff, sizeof(buff), "%lu", val2);

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 8 byte integer\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;          

            //case 0x08: 

            case 0x09: // 2 digit BCD (8 bit)
  
                val = (int)mbus_data_bcd_decode(record->data, 1); 
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val;
		
                snprintf(buff, sizeof(buff), "%d", val);     
        
                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 2 digit BCD\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;
                
            case 0x0A: // 4 digit BCD (16 bit)
        
                val = (int)mbus_data_bcd_decode(record->data, 2);  
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val;
		
                snprintf(buff, sizeof(buff), "%d", val);     

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 4 digit BCD\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;

            case 0x0B: // 6 digit BCD (24 bit)

                val = (int)mbus_data_bcd_decode(record->data, 3);  
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val;
		
                snprintf(buff, sizeof(buff), "%d", val);     

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 6 digit BCD\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;
                
            case 0x0C: // 8 digit BCD (32 bit)

                val = (int)mbus_data_bcd_decode(record->data, 4);  
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val;
		
                snprintf(buff, sizeof(buff), "%d", val);

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 8 digit BCD\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;
                                                                  
            case 0x0E: // 12 digit BCD (48 bit)

                val2 = mbus_data_bcd_decode(record->data, 6);  
		if (dataType != NULL)
		  *dataType = 0;
		if (value != NULL)
		  *value = (double)val2;
		
                snprintf(buff, sizeof(buff), "%lu", val2);

                if (debug)
                    printf("%s: DIF 0x%.2x was decoded using 12 digit BCD\n", __PRETTY_FUNCTION__, record->drh.dib.dif);

                break;

            case 0x0F: // special functions

                snprintf(buff, sizeof(buff), "Special functions");
                break;

            case 0x0D: // variable length
                if (record->data_len <= 0xBF)
                {
                    mbus_data_str_decode(buff, record->data, record->data_len);
                    break;
                }
                /* FALLTHROUGH */

            default:
        
		
                snprintf(buff, sizeof(buff), "Unknown DIF (0x%.2x)", record->drh.dib.dif);
                break;
        }

        return buff;     
    }

    return NULL;
}

double
simple_pow(int exp)
{
  int pot = 1;
      int sign;
      if (exp >= 0)
	sign = 1;
      else
      {
	sign = -1;
	exp = -exp;
      }
      
      while (exp > 0)
      {
	pot *= 10;
	exp--;
      }
      
      if (sign == 1)
	return (double)pot;
      else
	return 1.0/(double)pot;
}

const char *
mbus_data_record_value_and_unit(mbus_data_record* record)
{
    static char buff[128];
    int len = 0;
    double value;
    
    if (record)
    {
      //value and unit decoded togheter (using the prefix)
      int dataType;
      const char *valueString;
      const char *unitString;
      int prefix = 0;
      
      unitString = mbus_vib_unit_lookup2(&(record->drh.vib), &prefix);
      valueString = mbus_data_record_decode2(record, &dataType, &value);
      
      if (dataType == 0)
      {
	double scale_factor = simple_pow(prefix);
	value *= scale_factor;
	
	len += snprintf(&buff[len], sizeof(buff) - len, "%f\t", value);
	len += snprintf(&buff[len], sizeof(buff) - len, "%s\t", unitString);
      }
      else
      {
	len += snprintf(&buff[len], sizeof(buff) - len, "%s\t", valueString);
	len += snprintf(&buff[len], sizeof(buff) - len, "%f\t", unitString);
      }
      return buff;
    }
    
    return NULL;
}

//------------------------------------------------------------------------------
///
/// Encode string to CSV
///
//------------------------------------------------------------------------------
void  
mbus_str_csv_encode(u_char *dst, const u_char *src, size_t max_len)
{
  
}

//------------------------------------------------------------------------------
// Decode status
// 
// Code    	Meaning
// 00		No Error
// 01		Application Busy
// 10		Any Application Error
// 11		Reserved
//
// The Code is stored in header->status
//
///
/// Return a string containing the status
///
// See chapter 6.6
//
//------------------------------------------------------------------------------
const char *
mbus_data_variable_header_status(int status)
{
  static char buff[128];
  switch (status)
  {
    case 0b00: snprintf(buff, sizeof(buff), "%s", "No Error");
		break;
    case 0b01: snprintf(buff, sizeof(buff), "%s", "Application Busy");
		break;
    case 0b10: snprintf(buff, sizeof(buff), "%s", "Any Application Error");
		break;
    case 0b11: snprintf(buff, sizeof(buff), "%s", "Reserved");
		break;
  }
  
  return buff;
}

//------------------------------------------------------------------------------
// Decode value type
//
///
/// Return a integer holding the value type
///
// See chapter 8.4.3
//
//------------------------------------------------------------------------------
int
mbus_vif_value_type(u_char vif)
{
    static char buff[256];
    int n = 0;

    switch (vif & 0x7F) // ignore the extension bit in this selection
    {
        // E000 0nnn Energy 10(nnn-3) W
        case 0x00:
        case 0x00+1:
        case 0x00+2:
        case 0x00+3:
        case 0x00+4:
        case 0x00+5:
        case 0x00+6:
        case 0x00+7:
	  
            return 101;

        // 0000 1nnn          Energy       10(nnn)J     (0.001kJ to 10000kJ)
        case 0x08:
        case 0x08+1:
        case 0x08+2:
        case 0x08+3:
        case 0x08+4:
        case 0x08+5:
        case 0x08+6:
        case 0x08+7:
            
            return 0;        

        // E001 1nnn Mass 10(nnn-3) kg 0.001kg to 10000kg
        case 0x18:
        case 0x18+1:
        case 0x18+2:
        case 0x18+3:
        case 0x18+4:
        case 0x18+5:
        case 0x18+6:
        case 0x18+7:
            
            return 0;  

        // E010 1nnn Power 10(nnn-3) W 0.001W to 10000W
        case 0x28:
        case 0x28+1:
        case 0x28+2:
        case 0x28+3:
        case 0x28+4:
        case 0x28+5:
        case 0x28+6:
        case 0x28+7:

            return 201;
            
        // E011 0nnn Power 10(nnn) J/h 0.001kJ/h to 10000kJ/h
        case 0x30:
        case 0x30+1:
        case 0x30+2:
        case 0x30+3:
        case 0x30+4:
        case 0x30+5:
        case 0x30+6:
        case 0x30+7:

            return 0;     

        // E001 0nnn Volume 10(nnn-6) m3 0.001l to 10000l
        case 0x10:
        case 0x10+1:
        case 0x10+2:
        case 0x10+3:
        case 0x10+4:
        case 0x10+5:
        case 0x10+6:
        case 0x10+7:

            return 102;
        
        // E011 1nnn Volume Flow 10(nnn-6) m3/h 0.001l/h to 10000l/
        case 0x38:
        case 0x38+1:
        case 0x38+2:
        case 0x38+3:
        case 0x38+4:
        case 0x38+5:
        case 0x38+6:
        case 0x38+7:

            return 202;

        // E100 0nnn Volume Flow ext. 10(nnn-7) m3/min 0.0001l/min to 1000l/min
        case 0x40:
        case 0x40+1:
        case 0x40+2:
        case 0x40+3:
        case 0x40+4:
        case 0x40+5:
        case 0x40+6:
        case 0x40+7:

            return 0;

        // E100 1nnn Volume Flow ext. 10(nnn-9) m3/s 0.001ml/s to 10000ml/
        case 0x48:
        case 0x48+1:
        case 0x48+2:
        case 0x48+3:
        case 0x48+4:
        case 0x48+5:
        case 0x48+6:
        case 0x48+7:

            return 0; 

        // E101 0nnn Mass flow 10(nnn-3) kg/h 0.001kg/h to 10000kg/
        case 0x50:
        case 0x50+1:
        case 0x50+2:
        case 0x50+3:
        case 0x50+4:
        case 0x50+5:
        case 0x50+6:
        case 0x50+7:

            return 0;     

        // E101 10nn Flow Temperature 10(nn-3) °C 0.001°C to 1°C
        case 0x58:
        case 0x58+1:
        case 0x58+2:
        case 0x58+3:

            return 203;

        // E101 11nn Return Temperature 10(nn-3) °C 0.001°C to 1°C
        case 0x5C:
        case 0x5C+1:
        case 0x5C+2:
        case 0x5C+3:

            return 204;

        // E110 10nn Pressure 10(nn-3) bar 1mbar to 1000mbar
        case 0x68:
        case 0x68+1:
        case 0x68+2:
        case 0x68+3:

            return 0;;

        // E010 00nn On Time
        // nn = 00 seconds
        // nn = 01 minutes
        // nn = 10   hours
        // nn = 11    days
        // E010 01nn Operating Time coded like OnTime
        // E111 00nn Averaging Duration	coded like OnTime	 
        // E111 01nn Actuality Duration coded like OnTime
        case 0x20:
        case 0x20+1:
        case 0x20+2:
        case 0x20+3:
        case 0x24:
        case 0x24+1:
        case 0x24+2:
        case 0x24+3:
        case 0x70:
        case 0x70+1:
        case 0x70+2:
        case 0x70+3:
        case 0x74:
        case 0x74+1:
        case 0x74+2:
        case 0x74+3:
	  /*if      ((vif & 0x7C) == 0x20)
	      offset = snprintf(buff, sizeof(buff), "On time ");
	  else if ((vif & 0x7C) == 0x24) 
	      offset = snprintf(buff, sizeof(buff), "Operating time ");
	  else if ((vif & 0x7C) == 0x70)
	      offset = snprintf(buff, sizeof(buff), "Averaging Duration ");
	  else
	      offset = snprintf(buff, sizeof(buff), "Actuality Duration ");*/
	    return 0;                   

        // E110 110n Time Point
        // n = 0        date
        // n = 1 time & date
        // data type G
        // data type F
        case 0x6C:
        case 0x6C+1:

            return 0;
            
        // E110 00nn    Temperature Difference   10(nn-3)K   (mK to  K)
        case 0x60:
        case 0x60+1:
        case 0x60+2:
        case 0x60+3:

            return 0;

        // E110 01nn External Temperature 10(nn-3) °C 0.001°C to 1°C
        case 0x64:
        case 0x64+1:
        case 0x64+2:
        case 0x64+3:

           return 0;

        // E110 1110 Units for H.C.A. dimensionless
        case 0x6E:
            return 0;

        // E110 1111 Reserved
        case 0x6F:
            return 0;       

        // Custom VIF in the following string: never reached...
        case 0x7C:
            return 0;        

        // Fabrication No
        case 0x78:
            return 0;
            
        // Bus Address
        case 0x7A:
            return 0;

        // Manufacturer specific: 7Fh / FF  
        case 0x7F:
        case 0xFF:
            return 0;
                       
        default:
        
            return 0;
    }
}

//------------------------------------------------------------------------------
/// Generate CSV for variable-length data 
//------------------------------------------------------------------------------
char *
mbus_data_variable_csv(mbus_data_variable *data)
{
    mbus_data_record *record;
    mbus_data_variable_header *header;
    static char buff[8192];
    char str_encoded[256];
    size_t len = 0;
    size_t i;
    int val;
    
    if (data)
    {  
      header = &(data->header);     
       
      for (record = data->record, i = 0; record; record = record->next, i++)
      {
	  if (header) 
	  {
	    val = (int)mbus_data_bcd_decode(header->id_bcd, 4);
	    len += snprintf(&buff[len], sizeof(buff) - len, "%d\t", val);
	    len += snprintf(&buff[len], sizeof(buff) - len, "Kommentar\t"); 	    //comment
	    len += snprintf(&buff[len], sizeof(buff) - len, "%s\t",
	    mbus_decode_manufacturer(header->manufacturer[0], header->manufacturer[1]));
	    len += snprintf(&buff[len], sizeof(buff) - len, "%d\t", header->version);
	    len += snprintf(&buff[len], sizeof(buff) - len, "%d\t", header->access_no);
	    len += snprintf(&buff[len], sizeof(buff) - len, "%s\t", mbus_data_variable_medium_lookup(header->medium));
	  }
      
	  //len += snprintf(&buff[len], sizeof(buff) - len, "%s\t", mbus_data_record_value(record));
	  //len += snprintf(&buff[len], sizeof(buff) - len, "%s\t", mbus_data_record_unit(record));
	  len += snprintf(&buff[len], sizeof(buff) - len, "%s\t", mbus_data_record_value_and_unit(record));
	  time_t currenttime;
	  time (&currenttime);
	  struct tm *structured_time = localtime(&currenttime);
	  len += snprintf(&buff[len], sizeof(buff) - len, "%02d.%02d.%04d %02d:%02d:%02d\t", structured_time->tm_mday, 
										 structured_time->tm_mon+1, 
										 structured_time->tm_year + 1900,
										 structured_time->tm_hour,
										 structured_time->tm_min,
										 structured_time->tm_sec);//date time
	  len += snprintf(&buff[len], sizeof(buff) - len, "%d\t", mbus_vif_value_type(record->drh.vib.vif));//value type
	  len += snprintf(&buff[len], sizeof(buff) - len, "%d\t", 0); //memory number
	  len += snprintf(&buff[len], sizeof(buff) - len, "%d\t", header->status);
	  len += snprintf(&buff[len], sizeof(buff) - len, "%s\t", mbus_data_variable_header_status(header->status));//state german
	  
	  len += snprintf(&buff[len], sizeof(buff) - len, "\n");
	  //printf("%s\n", mbus_data_record_function(record));
	  //printf("%s\n", mbus_data_record_unit(record));
      }
      
      return buff;
    }
    
    return "";
}

//------------------------------------------------------------------------------
/// Generate CSV for fixed-length data 
//------------------------------------------------------------------------------
char *
mbus_data_fixed_csv(mbus_data_fixed *data)
{
  static char buff[8192];
  char str_encoded[256];
  size_t len = 0;

  if (data)
  {
      /*len += snprintf(&buff[len], sizeof(buff) - len, "<MBusData>\n\n");
  
      len += snprintf(&buff[len], sizeof(buff) - len, "    <SlaveInformation>\n");
      len += snprintf(&buff[len], sizeof(buff) - len, "        <Id>%d</Id>\n", (int)mbus_data_bcd_decode(data->id_bcd, 4));
      
      mbus_str_xml_encode(str_encoded, mbus_data_fixed_medium(data), sizeof(str_encoded)); 
      len += snprintf(&buff[len], sizeof(buff) - len, "        <Medium>%s</Medium>\n", str_encoded);
      
      len += snprintf(&buff[len], sizeof(buff) - len, "        <AccessNumber>%d</AccessNumber>\n", data->tx_cnt);
      len += snprintf(&buff[len], sizeof(buff) - len, "        <Status>%.2X</Status>\n", data->status);
      len += snprintf(&buff[len], sizeof(buff) - len, "    </SlaveInformation>\n\n");
	    
      len += snprintf(&buff[len], sizeof(buff) - len, "    <DataRecord id=\"0\">\n");
      
      mbus_str_xml_encode(str_encoded, mbus_data_fixed_function(data->status), sizeof(str_encoded));
      len += snprintf(&buff[len], sizeof(buff) - len, "        <Function>%s</Function>\n", str_encoded);
      
      mbus_str_xml_encode(str_encoded, mbus_data_fixed_unit(data->cnt1_type), sizeof(str_encoded));
      len += snprintf(&buff[len], sizeof(buff) - len, "        <Unit>%s</Unit>\n", str_encoded);
      if ((data->status & MBUS_DATA_FIXED_STATUS_FORMAT_MASK) == MBUS_DATA_FIXED_STATUS_FORMAT_BCD)
      {
	  len += snprintf(&buff[len], sizeof(buff) - len, "        <Value>%d</Value>\n", (int)mbus_data_bcd_decode(data->cnt1_val, 4));
      }
      else
      {
	  len += snprintf(&buff[len], sizeof(buff) - len, "        <Value>%d</Value>\n", mbus_data_int_decode(data->cnt1_val, 4));
      }
      len += snprintf(&buff[len], sizeof(buff) - len, "    </DataRecord>\n\n");      

      len += snprintf(&buff[len], sizeof(buff) - len, "    <DataRecord id=\"1\">\n");
      
      mbus_str_xml_encode(str_encoded, mbus_data_fixed_function(data->status), sizeof(str_encoded));
      len += snprintf(&buff[len], sizeof(buff) - len, "        <Function>%s</Function>\n", str_encoded);
      
      mbus_str_xml_encode(str_encoded, mbus_data_fixed_unit(data->cnt2_type), sizeof(str_encoded));
      len += snprintf(&buff[len], sizeof(buff) - len, "        <Unit>%s</Unit>\n", str_encoded);
      if ((data->status & MBUS_DATA_FIXED_STATUS_FORMAT_MASK) == MBUS_DATA_FIXED_STATUS_FORMAT_BCD)
      {
	  len += snprintf(&buff[len], sizeof(buff) - len, "        <Value>%d</Value>\n", (int)mbus_data_bcd_decode(data->cnt2_val, 4));
      }
      else
      {
	  len += snprintf(&buff[len], sizeof(buff) - len, "        <Value>%d</Value>\n", mbus_data_int_decode(data->cnt2_val, 4));
      }
      len += snprintf(&buff[len], sizeof(buff) - len, "    </DataRecord>\n\n");      

      len += snprintf(&buff[len], sizeof(buff) - len, "</MBusData>\n");

      return buff;*/
  }
  
  return "";
}

char *
mbus_frame_data_csv(mbus_frame_data *data)
{
  if (data)
  {
      if (data->type == MBUS_DATA_TYPE_FIXED)
      {
	  return mbus_data_fixed_csv(&(data->data_fix));
      }
      
      if (data->type == MBUS_DATA_TYPE_VARIABLE)
      {
	  return mbus_data_variable_csv(&(data->data_var));
      }
  }
  
  return "";
}

//------------------------------------------------------------------------------
/// Generate CSV for the variable-length data header
//------------------------------------------------------------------------------
char *
mbus_data_variable_header_csv(mbus_data_variable_header *header)
{
  static char buff[8192];
  char str_encoded[256];
  size_t len = 0;
  int val;
  
  if (header)
  { 
     return buff;
  }
  
  return ""; 
}