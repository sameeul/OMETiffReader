#include <chrono>
#include <memory>
#include <vector>
#include <iostream>
#include "ome_tiff_loader.h"
#include <sys/resource.h>

#include <numeric>

size_t partial_sum(std::shared_ptr<std::vector<uint32_t>> data, size_t x_min, size_t x_max, size_t y_min, size_t y_max)
{
    size_t sum = 0;
    for (int i=y_min; i<= y_max; ++i)
    {
        for (int j=x_min; j<=x_max; j++)
        {
            sum = sum + data->at(i*1024+j);
        }
    }

    return sum;
}



void test1()
{
    std::cout<<"Test 1 - Virtual Tile From 4 Tiles" <<std::endl;
    OmeTiffLoader imgLoader = OmeTiffLoader("r01_x10_y05_z08.ome.tif");
    auto numRowTiles = imgLoader.getRowTileCount();
    auto numColTiles = imgLoader.getColumnTileCount();  
    auto start = std::chrono::steady_clock::now(); 

    std::shared_ptr<std::vector<uint32_t>> tileData = imgLoader.getTileData(0,0);
    size_t sum = 0;
    sum += partial_sum(tileData, 500, 1023,700,1023);
    tileData = imgLoader.getTileData(0,1);
    sum += partial_sum(tileData, 0, 1500-1023,700,1023);
    tileData = imgLoader.getTileData(1,0);
    sum += partial_sum(tileData, 500, 1023,0,1400-1023);
    tileData = imgLoader.getTileData(1,1);
    sum += partial_sum(tileData, 0, 1500-1023,0,1400-1023);
    
    std::cout << "Manual Total :" << sum << std::endl;

    auto vTileData = imgLoader.getBoundingBoxVirtualTileData(700,1400, 500, 1500);
    size_t count = 0;
    sum = 0;
    for (auto x: *vTileData){
        sum +=x;
        count++;
    }
    std::cout <<"Virtual tile total: "<< sum<<std::endl;
    auto end = std::chrono::steady_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout<<"elapsed_time " << elapsed_seconds.count() << std::endl;

}


void test2()
{
    std::cout<<"Test 2 - Single Tile Subsection" <<std::endl;
    OmeTiffLoader imgLoader = OmeTiffLoader("r01_x10_y05_z08.ome.tif");
    auto numRowTiles = imgLoader.getRowTileCount();
    auto numColTiles = imgLoader.getColumnTileCount();  
    auto start = std::chrono::steady_clock::now(); 

    std::shared_ptr<std::vector<uint32_t>> tileData = imgLoader.getTileData(0,0);
    size_t sum = 0;
    sum += partial_sum(tileData, 0, 100,0,100);
   
    std::cout << "Manual Total :" << sum << std::endl;

    auto vTileData = imgLoader.getBoundingBoxVirtualTileData(0,100, 0, 100);
    size_t count = 0;
    sum = 0;
    for (auto x: *vTileData){
        sum +=x;
        count++;
    }
    std::cout <<"Virtual tile total: "<< sum<<std::endl;
    auto end = std::chrono::steady_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout<<"elapsed_time " << elapsed_seconds.count() << std::endl;

}

void test3()
{
    std::cout<<"Test 3 - Single Tile (Full)" <<std::endl;
    OmeTiffLoader imgLoader = OmeTiffLoader("r01_x10_y05_z08.ome.tif");
    auto numRowTiles = imgLoader.getRowTileCount();
    auto numColTiles = imgLoader.getColumnTileCount();  
    auto start = std::chrono::steady_clock::now(); 

    std::shared_ptr<std::vector<uint32_t>> tileData = imgLoader.getTileData(0,0);
    size_t sum = 0;
    sum += partial_sum(tileData, 0, 1023,0,1023);
   
    std::cout << "Manual Total :" << sum << std::endl;

    auto vTileData = imgLoader.getBoundingBoxVirtualTileData(0,1023, 0, 1023);
    size_t count = 0;
    sum = 0;
    for (auto x: *vTileData){
        sum +=x;
        count++;
    }
    std::cout <<"Virtual tile total: "<< sum<<std::endl;
    auto end = std::chrono::steady_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout<<"elapsed_time " << elapsed_seconds.count() << std::endl;

}


void test4()
{
    std::cout<<"Test 4 - Single Tile Memory usage" <<std::endl;
    OmeTiffLoader imgLoader = OmeTiffLoader("/mnt/hdd8/axle/dev/imgloader/build/r01_x10_y05_z08.ome.tif");
    struct rusage rss1, rss2;
    auto start = std::chrono::steady_clock::now(); 
    auto tmp = getrusage(RUSAGE_SELF, &rss1);
    std::shared_ptr<std::vector<uint32_t>> tileData = imgLoader.getTileData(0,0);
    tmp = getrusage(RUSAGE_SELF, &rss2);

    std::cout<<"Memory usage for tile " << rss2.ru_maxrss - rss1.ru_maxrss << std::endl;
    auto end = std::chrono::steady_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout<<"elapsed_time " << elapsed_seconds.count() << std::endl;

}

void test5()
{
    std::cout<<"Test 5 - Virtual Tile Stride" <<std::endl;
    OmeTiffLoader imgLoader = OmeTiffLoader("/mnt/hdd8/axle/dev/imgloader/build/r01_x10_y05_z08.ome.tif");

    std::shared_ptr<std::vector<uint32_t>> tileData1 = imgLoader.getBoundingBoxVirtualTileData(0,1079,0,1023);
    std::shared_ptr<std::vector<uint32_t>> tileData = imgLoader.getBoundingBoxVirtualTileDataStrideVersion(0,1079,3,0,1023,1);
    for(int i=0;i<10;i++){
        if (i%2==0){
            if(tileData1->at(i) != tileData->at(i/2)){std::cout<<"not match"<<std::endl;}
        }
    }
    size_t sum1 = std::accumulate(tileData->begin(), tileData->end(), 0);
    size_t sum2 = std::accumulate(tileData1->begin(), tileData1->end(), 0);
    std::cout << sum1 << " " << sum2<<std::endl;
}
int main(){
    //  test1();
    //  test2();
    //  test3();
    //  test4();
    test5();
    return 0;
}
