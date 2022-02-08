#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>




#define SPC_FREQ 32000.0

#define AMIGA_CLOCK 3579545.0



/*** compiled module format description:

byte - sequence size (song length)
$xx bytes - channel 0 seq
$xx bytes - channel 1 seq
$xx bytes - channel 2 seq
$xx bytes - channel 3 seq

byte - amount of patterns
for each pattern:
  byte - pattern size ($00 means $100)
  $xx bytes - pattern data

for 31 samples:
  word - length of sample in bytes (0 means empty)
  (only if size is nonzero)
    word - loop point of sample in bytes relative to start ($ffff means no loop)
    byte - sample volume
    byte - sample finetune
  $xxxx bytes - actual BRR data

***/


/* big-endian read routines */
uint16_t fget16(FILE *f)
{
  uint16_t v = 0;
  v |= fgetc(f)<<8;
  v |= fgetc(f)<<0;
  return v;
}

uint32_t fget32(FILE *f)
{
  uint32_t v = 0;
  v |= fgetc(f)<<24;
  v |= fgetc(f)<<16;
  v |= fgetc(f)<<8;
  v |= fgetc(f)<<0;
  return v;
}


/* little-endian write routines */
void fput16(uint16_t v,FILE *f)
{
  fputc((v>>0)&0xff, f);
  fputc((v>>8)&0xff, f);
}

void fput32(uint32_t v,FILE *f)
{
  fputc((v>>0)&0xff, f);
  fputc((v>>8)&0xff, f);
  fputc((v>>16)&0xff, f);
  fputc((v>>24)&0xff, f);
}



const uint16_t note_periods[] = {
  856,808,762,720,678,640,604,570,538,508,480,453,
  428,404,381,360,339,320,302,285,269,254,240,226,
  214,202,190,180,170,160,151,143,135,127,120,113
};


const int8_t finetune_tbl[] = {0,1,2,3,4,5,6,7,-8,-7,-6,-5,-4,-3,-2,-1};


typedef struct {
  unsigned length;
  int8_t finetune;
  uint8_t volume;
  unsigned loopstart;
  unsigned looplen;
  uint8_t loopflag;
  unsigned actuallen;
} sample_t;



int main(int argc, char *argv[])
{
  if (argc == 1 || (argc % 2 == 0))
  {
    puts("usage: convert [inname outname]...");
    return EXIT_FAILURE;
  }
  
  int fails = 0;
  for (int argindex = 1; argindex < argc; argindex += 2)
  {
    /* read module info */
    char *inname = argv[argindex];
    printf("Opening \"%s\"...",inname);
    FILE *inf = fopen(inname,"rb");
    if (!inf)
    {
      printf(strerror(errno));
      return EXIT_FAILURE;
    }
    puts("OK");
    char *outname = argv[argindex+1];
    
    int extdflag;
    fseek(inf,1080,SEEK_SET);
    char extdcode[4];
    fread(extdcode,1,4,inf);
    if (isgraph(extdcode[0])&&isgraph(extdcode[1])&&isgraph(extdcode[2])&&isgraph(extdcode[3]))
    {
      printf("Module code: \"%.4s\": assuming 31-sample module\n",extdcode);
      extdflag = 1;
    }
    else
    {
      puts("No module code. Assuming 15-sample module");
      extdflag = 0;
    }
    int maxsamps = extdflag ? 31 : 15;
    
    rewind(inf);
    char songname[20];
    fread(songname,1,20,inf);
    printf("Module name: \"%.20s\"\n", songname);
    
    sample_t samples[31];
    unsigned sample_size_total = 0;
    for (int i = 0; i < maxsamps; i++)
    {
      char sampname[22];
      fread(sampname,1,22,inf);
      unsigned length = fget16(inf);
      int finetune = fgetc(inf)&0xf;
      unsigned volume = fgetc(inf);
      unsigned loopstart = fget16(inf);
      unsigned looplen = fget16(inf);
      
      samples[i].length = length*2;
      samples[i].finetune = finetune;
      samples[i].volume = volume;
      samples[i].loopstart = loopstart*2;
      samples[i].looplen = looplen*2;
      
      samples[i].loopflag = looplen > 1;
      
      /* cut off any excess bytes after the loop end */
      if (samples[i].loopflag)
      {
        samples[i].actuallen = samples[i].loopstart+samples[i].looplen;
      }
      else
      {
        samples[i].actuallen = samples[i].length;
      }
      sample_size_total += samples[i].actuallen;
      
      printf("Sample %i: \"%.22s\" Length: %u Finetune: %i Volume: %u Loop point:%u/length:%u (%sloop)\n",
              i+1, sampname,
              samples[i].length,
              samples[i].finetune,
              samples[i].volume,
              samples[i].loopstart,
              samples[i].looplen,
              samples[i].loopflag ? "" : "no "
              );
    }
    for (int i = maxsamps; i < 31; i++)
      samples[i].length = 0;
    unsigned spc_sample_size_estimate = ceil(sample_size_total*(9.0/16.0));
    printf("Total sample size: %u bytes (~%u after BRR)\n",sample_size_total,spc_sample_size_estimate);
    
    unsigned songlength = fgetc(inf);
    fgetc(inf);
    printf("Song length: %u orders\n",songlength);
    uint8_t orderlist[128];
    fread(orderlist,1,128,inf);
    unsigned maxpatt = 0;
    unsigned fullmaxpatt = 0;
    /* yes, we do have to iterate through the ENTIRE orderlist, and not just the actual song bytes.
        e.g. "hardfight" has extra unused patterns after the song length */
    for (int i = 0; i < 128; i++)
    {
      uint8_t p = orderlist[i];
      if (p > maxpatt && i < songlength) maxpatt = p;
      if (p > fullmaxpatt) fullmaxpatt = p;
    }
    printf("Module has %u patterns\n", maxpatt+1);
    if (fullmaxpatt != maxpatt)
      printf("Unused patterns to up to %u\n", fullmaxpatt);
    
    
    FILE *outf = fopen(outname,"wb");
    if (!outf)
    {
      printf("Couldn't open %s: %s\n", outname,strerror(errno));
      goto in_fail;
    }
    
    
    
    /* read patterns */
    int ustflag = 0;
    if (extdflag) fseek(inf,4,SEEK_CUR);
    else
    {
      /* try to detect old ultimate soundtracker modules */
      /* reference: https://raw.githubusercontent.com/cmatsuoka/tracker-history/master/reference/amiga/soundtracker/Ultimate_Soundtracker-format.txt) */
      
      for (int i = 0; i < maxsamps; i++)
      {
        /* if any sample is >9999 bytes this is not a ust module */
        if (samples[i].length > 9999) goto not_ust;
        
        /* if any sample is finetuned this is not a ust module */
        if (samples[i].finetune) goto not_ust;
      }
      
      /* if the patterns contain any non-000/1xx/2xx effects this is not a ust module */
      size_t filepos = ftell(inf);
      for (int i = 0; i <= fullmaxpatt; i++)
      {
        for (int row = 0; row < 0x40; row++)
        {
          for (int chn = 0; chn < 4; chn++)
          {
            uint8_t ptnrow[4];
            fread(ptnrow,1,4,inf);
            
            uint8_t eff = ptnrow[2] & 0x0f;
            uint8_t param = ptnrow[3];
            
            if (eff > 2) goto not_ust_2;
            if (!eff && param) goto not_ust_2;
            
            /* additionally for any 2xx effects, ensure the param is x0 or 0y */
            if (eff == 2)
            {
              if ((param & 0x0f) && (param & 0xf0)) goto not_ust_2;
            }
          }
        }
      }
      
      ustflag = 1;
      puts("Ultimate SoundTracker module detected!");
      
      
  not_ust_2:
      fseek(inf,filepos,SEEK_SET);
      
  not_ust:
      ;
    }
    
    /* some non-ust modules still have their sample loop points in bytes, and we need to check for them */
    int correct_sample_flag = ustflag;
    if (!ustflag)
    {
      for (int i = 0; i < maxsamps; i++)
      {
        /* +16 to give a few bytes of safety */
        if (samples[i].loopflag  &&  samples[i].loopstart + samples[i].looplen > samples[i].length+16)
        {
          correct_sample_flag = 1;
          puts("Byte sample lengths detected!");
          break;
        }
      }
    }
    
    /* correct sample repeat offsets */
    if (correct_sample_flag)
    {
      for (int i = 0; i < maxsamps; i++)
      {
        if (samples[i].loopflag)
        {
          samples[i].loopstart /= 2;
          samples[i].actuallen = samples[i].loopstart+samples[i].looplen;
        }
      }
    }
    
    
    int outpattcnt = 0;
    uint8_t *outpattbuf = NULL;
    size_t outpattbufmax = 0;
    size_t outpattbufsize = 0;
    size_t outpattindexes[256];
    uint8_t outpattsizes[256];
    uint8_t outpattmap[4][0x40];
    
    for (int i = 0; i <= fullmaxpatt; i++)
    {
      /* don't bother with any patterns that are not referenced in the orderlist */
      if (!memchr(orderlist,i,songlength))
      {
        fseek(inf,0x40*4*4,SEEK_CUR);
        continue;
      }
      
      /* this table is accessed [chan][row][byte] */
      uint8_t patterndata[4][0x40][4];
      for (int row = 0; row < 0x40; row++)
      {
        for (int chn = 0; chn < 4; chn++)
        {
          fread(patterndata[chn][row],1,4,inf);
        }
      }
      
      /* detect repetitive patterns */
      uint8_t pattsize[4];
      memset(pattsize, 0x40, sizeof(pattsize));
      for (int chn = 0; chn < 4; chn++)
      {
        for (int cut = 0x20; cut > 1; cut /= 2)
        {
          if (memcmp(patterndata[chn][0],patterndata[chn][cut],cut*4)) break;
          pattsize[chn] = cut;
        }
      }
      
  /*** "packed" pattern format 

  patterns are separated into channels first

  $c0-$ff - duration change
  $a0-$bf - sample change
  $90-$9f - effects
  $8f - equivalent to 000 (blank) effect
  $00-$23 - notes
  $24 - blank note
  $40-$64 - same as $00-$24 but with sample reset
  $8e - return to start of pattern

  there is NO end-byte, all patterns are ended after $40 rows (or Bxx/D00 effect)
  $8e can be used for repetitive patterns

  ***/
      
      for (int chn = 0; chn < 4; chn++)
      {
        uint8_t packedpattern[256];
        int packedsize = 0;
        
        
        uint8_t prvdur = -1;
        uint8_t durcnt = 0;
        uint8_t prvsamp = -1;
        uint16_t prveff = -1;
        
        uint8_t notesave = -1;
        uint8_t sampsave = -1;
        uint16_t effsave = -1;
        
        uint8_t nextnote;
        uint8_t nextsamp;
        uint16_t nexteff;
        
        for (int row = 0; row <= pattsize[chn]; row++)
        {
          int newflag = 0;
          
          /* if the pattern is not over, get the next row */
          if (row < pattsize[chn])
          {
            uint8_t *rowdata = patterndata[chn][row];
            uint16_t period = rowdata[1] | ((rowdata[0]&0x0f)<<8);
            uint8_t sample = (rowdata[2]>>4) | (rowdata[0]&0xf0);
            uint8_t effcommand = rowdata[2]&0x0f;
            uint8_t effparam = rowdata[3];
            uint16_t effect = (effcommand<<8) | effparam;
            
            uint8_t note = -1;
            if (!period)
              note = 0x24;
            else
            {
              for (int i = 0; i < 0x24; i++)
              {
                if (period == note_periods[i])
                {
                  note = i;
                  break;
                }
              }
            }
            if (note > 0x24)
            {
              printf("ERROR: Pattern %i, row %i, channel %i: Bad period value %u\n",i,row,chn,period);
              goto out_fail;
            }
            if (sample > maxsamps)
            {
              printf("ERROR: Pattern %i, row %i, channel %i: Bad sample value %u\n",i,row,chn,sample);
              goto out_fail;
            }
            
            /* ultimate soundtracker modules have different effects */
            if (ustflag)
            {
              if (!effcommand) effect = 0;
              if (effcommand == 1) effect = 0x000 | effparam;
              if (effcommand == 2)
              {
                if (effparam & 0xf0)
                  effect = 0x200 | (effparam >> 4);
                else if (effparam & 0x0f)
                  effect = 0x100 | (effparam & 0x0f);
                else
                  effect = 0;
              }
            }
            
            /* if this is the first row, pre-initialize the information */
            if (!row)
            {
              notesave = note;
              sampsave = sample;
              effsave = effect;
            }
            else
            { /* otherwise check if we should write a new row */
              /* there are three conditions: note init, sample change, or effect change */
              if (note != 0x24 || sample > 0 || effect != effsave)
              {
                newflag = 1;
                nextnote = note;
                nextsamp = sample;
                nexteff = effect;
              }
            }
          }
          else
          {
            /* otherwise signify end of pattern */
            newflag = 2;
          }
          
          
          if (newflag)
          {
            int j = packedsize;
            uint8_t *p = packedpattern;
            
            /* duration change? */
            if (durcnt != prvdur)
            {
              p[j++] = (durcnt-1) | 0xc0;
              
              prvdur = durcnt;
            }
            /* sample change?? */
            if (sampsave && sampsave != prvsamp)
            {
              p[j++] = sampsave | 0xa0;
              prvsamp = sampsave;
            }
            /* effect? */
            if (effsave != prveff)
            {
              if (!effsave)
                p[j++] = 0x8f;
              else
              {
                p[j++] = (effsave>>8) | 0x90;
                p[j++] = (effsave&0xff);
              }
              
              prveff = effsave;
            }
            /* we always want a note */
            p[j++] = notesave | (sampsave ? 0x40 : 0);
            
            if (newflag == 1)
            { /* not end of pattern */
              notesave = nextnote;
              sampsave = nextsamp;
              effsave = nexteff;
            }
            else
            { /* end of pattern */
              if (pattsize[chn] != 0x40) /* put a repeat marker for repetitive patterns */
                p[j++] = 0x8e;
            }
            
            if (j > 256)
            {
              printf("ERROR: Pattern %i, channel %i packed data is too large!\n",i,chn);
              goto out_fail;
            }
            
            durcnt = 0;
            packedsize = j;
          }
          durcnt++;
        }
        
        
        /* check if this pattern is a duplicate */
        uint8_t outpattid = 0;
        for ( ; outpattid < outpattcnt; outpattid++)
        {
          if (packedsize == outpattsizes[outpattid] && !memcmp(packedpattern,outpattbuf+outpattindexes[outpattid],packedsize))
            break;
        }
        
        /* if it isn't, allocate space for it */
        if (outpattid == outpattcnt)
        {
          outpattindexes[outpattid] = outpattbufsize;
          outpattsizes[outpattid] = packedsize-1;
          
          size_t newsize = outpattbufsize + packedsize;
          if (newsize > outpattbufmax)
          {
            if (!outpattbufmax) outpattbufmax = 0x1000;
            else outpattbufmax *= 2;
            outpattbuf = realloc(outpattbuf,outpattbufmax);
          }
          memcpy(outpattbuf+outpattbufsize,packedpattern,packedsize);
          
          outpattbufsize = newsize;
          outpattcnt++;
        }
        
        /* set the id in the map */
        outpattmap[chn][i] = outpattid;
      }
    }
    
    /* output patterns */
    fputc(outpattcnt & 0xff,outf);
    for (int i = 0; i < outpattcnt; i++)
    {
      unsigned size = outpattsizes[i]+1;
      uint8_t *p = outpattbuf+outpattindexes[i];
      
      fputc(size & 0xff,outf);
      fwrite(p,1,size,outf);
    }
    free(outpattbuf);
    
    
    /* output sequences */
    fputc(songlength & 0xff,outf);
    for (int chn = 0; chn < 4; chn++)
    {
      for (int i = 0; i < songlength; i++)
        fputc(outpattmap[chn][orderlist[i]], outf);
    }
    
    
    
    /* read samples */
    for (int i = 0; i < 31; i++)
    {
      unsigned length = samples[i].length;
      unsigned loopstart = samples[i].loopstart;
      unsigned loopflag = samples[i].loopflag;
      unsigned actuallen = samples[i].actuallen;
      
      if (length)
      {
        int8_t *data = malloc(length);
        fread(data,1,length,inf);
        
        unsigned loopblock = loopstart/16;
        if (!loopflag)
          loopblock = -1;
        unsigned lastblock = (actuallen-1)/16;
        
        fput16((lastblock+1)*9, outf);
        fput16(loopblock == -1 ? 0xffff : (loopblock*9), outf);
        fputc(samples[i].volume,outf);
        fputc(samples[i].finetune,outf);
        
        int initial_old = 0;
        int initial_older = 0;
        for (unsigned blocknum = 0; blocknum <= lastblock; blocknum++)
        {
          unsigned srcindex = blocknum*16;
          
          uint8_t blockhead = (blocknum == lastblock) ? (loopflag ? 3 : 1) : 0;
          
          /* all these tables are indexed [filter][shift] */
          int8_t outnibs[4][12][16];
          int olders[4][12];
          int olds[4][12];
          
          unsigned minerror = UINT_MAX;
          unsigned bestshift = -1;
          unsigned bestfilter = -1;
          
          /* force filter 0 on initial block and loop block */
          for (int filter = 0; filter < ((blocknum == 0 || blocknum == loopblock) ? 1 : 4); filter++)
          {
            /* spc shifts left n+1 times and then shifts right once */
            /* therefore this number must be +1 in the block header */
            for (int shift = 0; shift < 12; shift++)
            {
              int old = initial_old;
              int older = initial_older;
              
              unsigned error = 0;
              
              for (int samp = 0; samp < 16; samp++)
              {
                /* find the best nibble for each sample */
                int target = (srcindex+samp >= length) ? data[length-1] : data[srcindex+samp];
                target <<= (14-8);
                
                int new;
                
                int filter_coeff;
                switch (filter)
                {
                  case 0:
                    filter_coeff = 0;
                    break;
                  case 1:
                    filter_coeff = old*0.9375;
                    break;
                  case 2:
                    filter_coeff = old*1.90625 - older*0.9375;
                    break;
                  case 3:
                    filter_coeff = old*1.796875 - older*0.8125;
                    break;
                }
                
                /* sample = nibble << shift */
                /* new = sample + filter_coeff */
                /* nibble << shift = new - filter_coeff */
                /* nibble * (1 << shift) = new - filter_coeff */
                /* nibble = (new - filter_coeff) / (1 << shift) */
                /* we use this and round to find the best nibble in the range */
                int nibble = round(((double)target - (double)filter_coeff) / (double)(1 << shift));
                
                /* there are some situations where we need to clamp the nibble: */
                /*  1: the nibble is outside of -8..7 */
                /*  2: the resulting sample is outside of -$3ffa..$3ff8 */
                if (nibble > 7) nibble = 7;
                if (nibble < -8) nibble = -8;
                
                /* ok, nibble is in range, now get the sample value it would create */
                new = (nibble << shift) + filter_coeff;
                /* if new is outside of -$3ffa..$3ff8 we need to get it back in there */
                while (new > 0x3ff8)
                {
                  nibble--;
                  new -= (1<<shift);
                }
                while (new < -0x3ffa)
                {
                  nibble++;
                  new += (1<<shift);
                }
                
                /* check if it's impossible to find a working nibble */
                if (nibble < -8 || nibble > 7) goto next_sample_mode;
                
                /* we now have a sample, get the error */
                error += abs(target-new);
                
                outnibs[filter][shift][samp] = nibble;
                
                older = old;
                old = new;
              }
              
              olders[filter][shift] = older;
              olds[filter][shift] = old;
              
              if (error < minerror)
              {
                minerror = error;
                bestshift = shift;
                bestfilter = filter;
              }
              
  next_sample_mode:
              continue;
            }
          }
          
          
          blockhead |= bestfilter<<2;
          blockhead |= (bestshift+1)<<4;
          int8_t *n = outnibs[bestfilter][bestshift];
          
          fputc(blockhead,outf);
          fputc(((n[0]&0x0f)<<4) | (n[1]&0x0f),outf);
          fputc(((n[2]&0x0f)<<4) | (n[3]&0x0f),outf);
          fputc(((n[4]&0x0f)<<4) | (n[5]&0x0f),outf);
          fputc(((n[6]&0x0f)<<4) | (n[7]&0x0f),outf);
          fputc(((n[8]&0x0f)<<4) | (n[9]&0x0f),outf);
          fputc(((n[10]&0x0f)<<4) | (n[11]&0x0f),outf);
          fputc(((n[12]&0x0f)<<4) | (n[13]&0x0f),outf);
          fputc(((n[14]&0x0f)<<4) | (n[15]&0x0f),outf);
          
          
          initial_older = olders[bestfilter][bestshift];
          initial_old = olds[bestfilter][bestshift];
        }
        
        
        free(data);
      }
      else
      {
        fput16(0,outf);
      }
    }
    
    
    
    fclose(inf);
    fclose(outf);
    continue;
    
  in_fail:
    fclose(inf);
    fails++;
    continue;
    
  out_fail:
    fclose(inf);
    fclose(outf);
    remove(outname);
    fails++;
    continue;
  }
  
  if (fails) return EXIT_FAILURE;
  else return EXIT_SUCCESS;
}