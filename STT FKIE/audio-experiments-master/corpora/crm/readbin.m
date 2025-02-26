function out = readbin(name)
%import binary sound files
fp=fopen(name,'rb');
bin=fread(fp,'integer*2');
fclose(fp);
out = (bin/16384);