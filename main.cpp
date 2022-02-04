#include <memory>
#include <vector>
#include <iostream>
#include "ome_tiff_loader.h"

int main(){
    OmeTiffLoader imgLoader = OmeTiffLoader("small_test_image_1.tif");
    auto numRowTiles = imgLoader.getRowTileCount();
    auto numColTiles = imgLoader.getColumnTileCount();  

    for (auto i=0; i<numRowTiles; ++i){
        for(auto j=0; j<numColTiles; ++j){
            std::shared_ptr<std::vector<uint32_t>> tileData = imgLoader.getTileData(i,j);
            uint32_t sum = 0;
            for (auto x: *tileData){
                sum+= x;
            }

            std::cout <<"Row: "<<i <<" Col: "<<j<<" Sum: "<<sum<<std::endl;
        }
    }  
    return 0;
}