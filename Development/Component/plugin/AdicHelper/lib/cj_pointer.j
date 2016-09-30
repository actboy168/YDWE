// Edit By TigerCN
library Indicator {

    int array Trend_Guide;
    int ReplaceA, ReplaceB;

    struct Pointer {
    
        int GetSite(float r) { return r; return 0; }
    
        int NewSite(int n) {
            float r = I2R(n); int i, j = GetSite(r), k = j - (j / 8192) * 8192;
            if (k < 0) { k += 8192; }
            i = k;
            do {
                if (Trend_Guide[i] == 0) {
                    Trend_Guide[i] = R2I(r);
                    return i;
                }
                elseif (i < 8191) {
                    i ++;
                }
                else {
                    i = 0;
                }
            }
            while(i != k);
            return -1;
        }
    
        void SwapSite(Block block, int i, int j) {
            int media = block[j];
            block[j] = block[i];
            block[i] = media;
        }

        void SwapDigit(int p1, int p2) {
            int i = p1
            p1 = p2
            p2 = i
            ReplaceA = p1; 
            ReplaceB = p2;
        } 
    
        void ClearSite(int id) { Trend_Guide[id] = 0; }

    }   

    

}

define {

    SwapFinger(p1,p2) = { SwapDigit(p1,p2); p1 = ReplaceA; p2 = ReplaceB }

}