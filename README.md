# ShannonFanoCoding

是完整的壓縮工具，不過現階段只能壓縮/解壓縮一個檔案，有時間會設計成對整個資料夾內容進行壓縮並還原(目前構思是在header上刻目錄)

總共包含5個部分

[ShannonFanoToo]控管整個壓縮/解壓縮流程的對外用API：
1.流程控制
2.壓縮/解壓縮的進度
3.壓縮檔header包裝及版本控管

[ShannonFanoCompress]壓縮主程式：
1.根據SFTree將原始資料存入壓縮檔內
2.不定長度bit的儲存流程設計
3.壓縮資料總長度紀錄及結尾資料包裝

[ShannonFanoDecompress]解壓縮主程式：
1.根據SFTree將壓縮檔還原成原始資料
2.不定長度bit的儲存流程設計
3.SFTree狀態機讀取設計

[ShannonFanoTree]SFTree核心程式：
1.讀取counter
2.SFTree核心演算法設計

[FreqCount]位元計數器：
1.讀原始檔並紀錄各位元的出現總次數
2.讀壓縮檔並抓出header中的次數統計資料
