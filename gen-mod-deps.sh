outname="mod-deps.txt"

echo -n "OUT_MODS := " > $outname

for fnam in mods/*
do
  # remove spaces from filenames
  newname=${fnam// /_}
  if [ "$fnam" != "$newname" ];
  then mv "$fnam" "$newname"
  fi
  
  # send it to the makefile
  echo -n " cmods/${newname#mods/}.cmod" >> $outname
done
