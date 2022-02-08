#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>


#define OUTNAME "generated-data.asm"


#define SPC_FREQ 32000.0

#define MIN_PERIOD 108
#define MAX_PERIOD 907
#define AMT_PERIODS (MAX_PERIOD-MIN_PERIOD+1)

#define AMIGA_CLOCK 3579545.0


const uint16_t note_periods[][12*3] = {
  {
    856,808,762,720,678,640,604,570,538,508,480,453,
    428,404,381,360,339,320,302,285,269,254,240,226,
    214,202,190,180,170,160,151,143,135,127,120,113
  }, {
    850,802,757,715,674,637,601,567,535,505,477,450, // C-1 to B-1 Finetune +1
    425,401,379,357,337,318,300,284,268,253,239,225, // C-2 to B-2 Finetune +1
    213,201,189,179,169,159,150,142,134,126,119,113 // C-3 to B-3 Finetune +1
  }, {
    844,796,752,709,670,632,597,563,532,502,474,447, // C-1 to B-1 Finetune +2
    422,398,376,355,335,316,298,282,266,251,237,224, // C-2 to B-2 Finetune +2
    211,199,188,177,167,158,149,141,133,125,118,112 // C-3 to B-3 Finetune +2
  }, {
    838,791,746,704,665,628,592,559,528,498,470,444, // C-1 to B-1 Finetune +3
    419,395,373,352,332,314,296,280,264,249,235,222, // C-2 to B-2 Finetune +3
    209,198,187,176,166,157,148,140,132,125,118,111 // C-3 to B-3 Finetune +3
  }, {
    832,785,741,699,660,623,588,555,524,495,467,441, // C-1 to B-1 Finetune +4
    416,392,370,350,330,312,294,278,262,247,233,220, // C-2 to B-2 Finetune +4
    208,196,185,175,165,156,147,139,131,124,117,110 // C-3 to B-3 Finetune +4
  }, {
    826,779,736,694,655,619,584,551,520,491,463,437, // C-1 to B-1 Finetune +5
    413,390,368,347,328,309,292,276,260,245,232,219, // C-2 to B-2 Finetune +5
    206,195,184,174,164,155,146,138,130,123,116,109 // C-3 to B-3 Finetune +5
  }, {
    820,774,730,689,651,614,580,547,516,487,460,434, // C-1 to B-1 Finetune +6
    410,387,365,345,325,307,290,274,258,244,230,217, // C-2 to B-2 Finetune +6
    205,193,183,172,163,154,145,137,129,122,115,109 // C-3 to B-3 Finetune +6
  }, {
    814,768,725,684,646,610,575,543,513,484,457,431, // C-1 to B-1 Finetune +7
    407,384,363,342,323,305,288,272,256,242,228,216, // C-2 to B-2 Finetune +7
    204,192,181,171,161,152,144,136,128,121,114,108 // C-3 to B-3 Finetune +7
  }, {
    907,856,808,762,720,678,640,604,570,538,504,480, // C-1 to B-1 Finetune -8
    453,428,404,381,360,339,320,302,285,269,254,240, // C-2 to B-2 Finetune -8
    226,214,202,190,180,170,160,151,143,135,127,120 // C-3 to B-3 Finetune -8
  }, {
    900,850,802,757,715,675,636,601,567,535,505,477, // C-1 to B-1 Finetune -7
    450,425,401,379,357,337,318,300,284,268,253,238, // C-2 to B-2 Finetune -7
    225,212,200,189,179,169,159,150,142,134,126,119 // C-3 to B-3 Finetune -7
  }, {
    894,844,796,752,709,670,632,597,563,532,502,474, // C-1 to B-1 Finetune -6
    447,422,398,376,355,335,316,298,282,266,251,237, // C-2 to B-2 Finetune -6
    223,211,199,188,177,167,158,149,141,133,125,118 // C-3 to B-3 Finetune -6
  }, {
    887,838,791,746,704,665,628,592,559,528,498,470, // C-1 to B-1 Finetune -5
    444,419,395,373,352,332,314,296,280,264,249,235, // C-2 to B-2 Finetune -5
    222,209,198,187,176,166,157,148,140,132,125,118 // C-3 to B-3 Finetune -5
  }, {
    881,832,785,741,699,660,623,588,555,524,494,467, // C-1 to B-1 Finetune -4
    441,416,392,370,350,330,312,294,278,262,247,233, // C-2 to B-2 Finetune -4
    220,208,196,185,175,165,156,147,139,131,123,117 // C-3 to B-3 Finetune -4
  }, {
    875,826,779,736,694,655,619,584,551,520,491,463, // C-1 to B-1 Finetune -3
    437,413,390,368,347,338,309,292,276,260,245,232, // C-2 to B-2 Finetune -3
    219,206,195,184,174,164,155,146,138,130,123,116 // C-3 to B-3 Finetune -3
  }, {
    868,820,774,730,689,651,614,580,547,516,487,460, // C-1 to B-1 Finetune -2
    434,410,387,365,345,325,307,290,274,258,244,230, // C-2 to B-2 Finetune -2
    217,205,193,183,172,163,154,145,137,129,122,115 // C-3 to B-3 Finetune -2
  }, {
    862,814,768,725,684,646,610,575,543,513,484,457, // C-1 to B-1 Finetune -1
    431,407,384,363,342,323,305,288,272,256,242,228, // C-2 to B-2 Finetune -1
    216,203,192,181,171,161,152,144,136,128,121,114 // C-3 to B-3 Finetune -1
  }
};


int main()
{
  FILE *outf = fopen(OUTNAME,"wb");
  if (!outf)
  {
    puts(strerror(errno));
    return EXIT_FAILURE;
  }
  
  /* make amiga tempo->spc timer conversion table */
  fprintf(outf,"timer_tbl: .db ");
  for (int i = 0x20; i < 256; i++)
  {
    /* there are 4 rows per beat */
    double rpm = i*4;
    /* there are 6 ticks per row */
    double tpm = rpm*6;
    /* now get ticks per second (the target Hz value) */
    double rate = tpm/60;
    
    unsigned timer = round(8000.0 / rate);
    fprintf(outf,"$%02x,", timer < 0x100 ? timer : 0xff);
  }
  
  /* make amiga period->spc freq conversion table */
  uint16_t freqs[AMT_PERIODS];
  for (int i = 0; i < AMT_PERIODS; i++)
  {
    uint16_t period = i+MIN_PERIOD;
    double freq = AMIGA_CLOCK / period;
    freqs[i] = round((0x1000 * freq) / SPC_FREQ);
  }
  fprintf(outf,"\n\nfreq_tbl: .dw ");
  for (int i = 0; i < AMT_PERIODS; i++)
    fprintf(outf,"$%04x,",freqs[i]);
  
  /* and also note->period tables */
  fprintf(outf, "\n\nperiod_tbl_lo: .db ");
  for (int ft = 0; ft < 0x10; ft++)
    fprintf(outf, "<period_tbl_%x,", ft);
  fprintf(outf, "\nperiod_tbl_hi: .db ");
  for (int ft = 0; ft < 0x10; ft++)
    fprintf(outf, ">period_tbl_%x,", ft);
  
  for (int ft = 0; ft < 0x10; ft++)
  {
    fprintf(outf,"\nperiod_tbl_%x: .dw ", ft);
    for (int n = 0; n < 12*3; n++)
      fprintf(outf,"%i,",note_periods[ft][n]-MIN_PERIOD);
  }
  
  /* and also note arpeggiation factor table */
  uint16_t arptbl[15];
  for (int i = 0; i < 15; i++)
  {
    /* arpfreq = freq + (freq * f-1.0) */
    double f = pow(2.0,(i+1)/12.0);
    arptbl[i] = round((f-1.0) * (1<<15));
  }
  fprintf(outf,"\n\narp_tbl_lo: .db ");
  for (int i = 0; i < 15; i++)
    fprintf(outf,"$%02x,",(arptbl[i]&0xff));
  fprintf(outf,"\narp_tbl_hi: .db ");
  for (int i = 0; i < 15; i++)
    fprintf(outf,"$%02x,",(arptbl[i]>>8));
  
  
  fclose(outf);
}