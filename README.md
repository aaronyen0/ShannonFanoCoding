# ShannonFanoCoding (壓縮 / 解壓縮)

- 注意事項
  - 現階段只能壓縮/解壓縮一個檔案
  - 有時間會設計成能對整個資料夾內容進行壓縮並還原

## 程式架構
- ShannonFanoToo: 控管整個壓縮/解壓縮流程的對外用API(此部分程式碼似乎遺失了)
    - 流程控制
    - 壓縮/解壓縮的進度
    - 壓縮檔header包裝及版本控管

- ShannonFanoCompress: 壓縮主程式
    - 根據SFTree將原始資料存入壓縮檔內
    - 不定長度bit的儲存流程設計
    - 壓縮資料總長度紀錄及結尾資料包裝

- ShannonFanoDecompress: 解壓縮主程式
    - 根據SFTree將壓縮檔還原成原始資料
    - 不定長度bit的儲存流程設計
    - SFTree狀態機讀取設計

- ShannonFanoTree: SFTree核心程式
    - 讀取counter
    - SFTree核心演算法設計

- FreqCount: 位元計數器
    - 讀原始檔並紀錄各位元的出現總次數
    - 讀壓縮檔並抓出header中的次數統計資料
