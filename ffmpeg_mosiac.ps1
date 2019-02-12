# create 25 game of life videos each of 5 sec
For ($i=0; $i -le 24; $i++) {
    ffmpeg -f lavfi -i "life=s=60x60:death_color='random':life_color='random':ratio=.5:mold=10:size=ntsc"-sws_flags neighbor -vf scale=640:480,drawgrid=w=6:h=6 -t 5 -pix_fmt yuv420p lifegame$i.mp4
    }
# covert these to ts files and concat 
$files = ""
For ($j=0; $j -le 24; $j++) {
    ffmpeg -i lifegame$j.mp4 -c copy -bsf:v h264_mp4toannexb -f mpegts lifegame$j.ts
    $files += "lifegame$j.ts|"
    }
ffmpeg -i "concat:$files" -c copy -bsf:a aac_adtstoasc lifegame.mp4
# convert original bb videos to fast-forwarded
ffmpeg -i bb1.mp4 -filter_complex "[0:v]setpts=0.333*PTS[v];[0:a]atempo=3.0[a]" -map "[v]" -map "[a]" bb1fast.mp4
ffmpeg -i bb2.mp4 -filter_complex "[0:v]setpts=0.333*PTS[v];[0:a]atempo=3.0[a]" -map "[v]" -map "[a]" bb2fast.mp4
ffmpeg -i bb3.mp4 -filter_complex "[0:v]setpts=0.333*PTS[v];[0:a]atempo=3.0[a]" -map "[v]" -map "[a]" bb3fast.mp4
ffmpeg -i bb4.mp4 -filter_complex "[0:v]setpts=0.23*PTS[v];[0:a]atempo=4.3[a]" -map "[v]" -map "[a]" bb4fast.mp4
# combine 4 fast-forwarded videos to mosaic
ffmpeg -i bb1fast.mp4 -i bb2fast.mp4 -i bb3fast.mp4 -i bb4fast.mp4 -filter_complex "nullsrc=size=640x480 [base];[0:v] setpts=PTS-STARTPTS, scale=320x240 [upperleft];[1:v] setpts=PTS-STARTPTS, scale=320x240 [upperright];[2:v] setpts=PTS-STARTPTS, scale=320x240 [lowerleft];[3:v] setpts=PTS-STARTPTS, scale=320x240 [lowerright];[base][upperleft] overlay=shortest=1 [tmp1];[tmp1][upperright] overlay=shortest=1:x=320 [tmp2];[tmp2][lowerleft] overlay=shortest=1:y=240 [tmp3];[tmp3][lowerright] overlay=shortest=1:x=320:y=240" -c:v libx264 bbcombined.mp4
# extract audio from fast forwarded videos
ffmpeg -i bb1fast.mp4 bb1fast.mp3
ffmpeg -i bb2fast.mp4 bb2fast.mp3
ffmpeg -i bb3fast.mp4 bb3fast.mp3
ffmpeg -i bb4fast.mp4 bb4fast.mp3
# mix all audio
ffmpeg -i bb1fast.mp3 -i bb2fast.mp3 -i bb3fast.mp3 -i bb4fast.mp3 -filter_complex amix=inputs=4:duration=first mixed.mp3
# strip audio from mosaic & add mixed audio to mosaic
ffmpeg -i bbcombined.mp4 -i mixed.mp3 -map 0:0 -map 1:0 -c:v copy -c:a aac -b:a 256k -shortest bbmixed.mp4
# blend bb videos and game of life
ffmpeg -n -i bbmixed.mp4 -i lifegame.mp4 -filter_complex "[0:v]setsar=sar=1[v];[v][1]blend=all_mode='hardmix':all_opacity=0.5" outputhardmix.mp4
# more chaotic...
ffmpeg -n -i bbmixed.mp4 -i lifegame.mp4 -filter_complex "[0:v]setsar=sar=1[v];[v][1]blend=all_mode='hardmix':c0_opacity=1:c1_opacity=0.5" outputmorehardmix.mp4