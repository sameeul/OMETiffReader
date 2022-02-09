
#include "ome_tiff_loader.h"
#include <numeric>

OmeTiffLoader::OmeTiffLoader(const std::string &fNameWithPath){
    if (checkTileStatus(fNameWithPath))
		{
			gsTiffTileLoader = std::make_unique<GrayscaleTiffTileLoader<uint32_t>>(nThreads, fNameWithPath);
		} 
		else 
		{
			// since the file is not tiled, we provide the tile dimensions
			auto tileDims = calculateTileDimensions(fNameWithPath); //vector of (tw, th, td)
            gsTiffTileLoader = std::make_unique<GrayscaleTiffStripLoader<uint32_t>>(nThreads, fNameWithPath, tileDims->at(0), 
																						tileDims->at(1), tileDims->at(2));
		}
};

OmeTiffLoader::~OmeTiffLoader(){gsTiffTileLoader = nullptr;};

size_t OmeTiffLoader::getRowTileCount() const {return gsTiffTileLoader->numberTileHeight();}
size_t OmeTiffLoader::getColumnTileCount() const {return gsTiffTileLoader->numberTileWidth();}
size_t OmeTiffLoader::getImageHeight() const {return gsTiffTileLoader->fullHeight(0);}
size_t OmeTiffLoader::getImageWidth() const {return gsTiffTileLoader->fullWidth(0);}
size_t OmeTiffLoader::getTileHeight() const {return gsTiffTileLoader->tileHeight(0);}
size_t OmeTiffLoader::getTileWidth() const {return gsTiffTileLoader->tileWidth(0);}

std::shared_ptr<std::vector<uint32_t>> OmeTiffLoader::getTileData(size_t const indexRowGlobalTile, size_t const indexColGlobalTile)
{
    auto tw = gsTiffTileLoader->tileWidth(0);
    auto th = gsTiffTileLoader->tileHeight(0);
    std::shared_ptr<std::vector<uint32_t>> tileData = std::make_shared<std::vector<uint32_t>>(tw * th);
    gsTiffTileLoader->loadTileFromFile(tileData, indexRowGlobalTile, indexColGlobalTile, 0, 0);
    return tileData;
}

std::unique_ptr<std::vector<size_t>>  OmeTiffLoader::getImageDimensions  (const std::string& filePath) const
{
	TIFF *tiff_ = TIFFOpen(filePath.c_str(), "r");
	if (tiff_ != nullptr) 
	{
		std::unique_ptr<std::vector<size_t>> imageDims = std::make_unique<std::vector<size_t>>(0);
		size_t tmp = 0;
		TIFFGetField(tiff_, TIFFTAG_IMAGEWIDTH, &tmp);
		imageDims->push_back(tmp);
      	TIFFGetField(tiff_, TIFFTAG_IMAGELENGTH, &tmp);
		imageDims->push_back(tmp);
		imageDims->push_back(TIFFNumberOfDirectories(tiff_));
	   	TIFFClose(tiff_);
	  	return std::move(imageDims);	
	} else { throw (std::runtime_error("Tile Loader ERROR: The file can not be opened.")); }
}

std::unique_ptr<std::vector<size_t>>  OmeTiffLoader::calculateTileDimensions(const std::string& filePath) const
{
	auto imageDims = getImageDimensions(filePath);
	size_t defaultWidthSize = 1024;
	size_t defaultHeightSize = 1024;
	size_t defaultDepthSize = 1;
	imageDims->at(0) = std::min({imageDims->at(0), defaultWidthSize});
	imageDims->at(1) = std::min({imageDims->at(1), defaultHeightSize});
	imageDims->at(2) = std::min({imageDims->at(2), defaultDepthSize});
	return std::move(imageDims);
}

bool OmeTiffLoader::checkTileStatus(const std::string& filePath) const
{
	TIFF *tiff_ = TIFFOpen(filePath.c_str(), "r");
	if (tiff_ != nullptr) 
	{
		if (TIFFIsTiled(tiff_) == 0) 
		{ 
			TIFFClose(tiff_);
			return false;
			} else 
			{
			TIFFClose(tiff_);
			return true;
			}
	} else { throw (std::runtime_error("Tile Loader ERROR: The file can not be opened.")); }
}

std::shared_ptr<std::vector<uint32_t>> OmeTiffLoader::getTileData(size_t const indexGlobalTile)
{
	size_t columnTileCount = getColumnTileCount();	
	size_t indexRowGlobalTile = indexGlobalTile/columnTileCount;
	size_t indexColGlobalTile = indexGlobalTile%columnTileCount;
    auto tw = gsTiffTileLoader->tileWidth(0);
    auto th = gsTiffTileLoader->tileHeight(0);
    std::shared_ptr<std::vector<uint32_t>> tileData = std::make_shared<std::vector<uint32_t>>(tw * th);
    gsTiffTileLoader->loadTileFromFile(tileData, indexRowGlobalTile, indexColGlobalTile, 0, 0);
    return tileData;
}

std::shared_ptr<std::vector<uint32_t>> OmeTiffLoader::getTileDataContainingPixel(size_t const indexRowPixel, size_t const indexColPixel)
{
	size_t th = getTileHeight();	
	size_t tw = getTileWidth();
	size_t indexRowGlobalTile = indexRowPixel/th;
	size_t indexColGlobalTile = indexColPixel/tw;
    std::shared_ptr<std::vector<uint32_t>> tileData = std::make_shared<std::vector<uint32_t>>(tw * th);
    gsTiffTileLoader->loadTileFromFile(tileData, indexRowGlobalTile, indexColGlobalTile, 0, 0);
    return tileData;
}


std::pair<size_t, size_t> OmeTiffLoader::getTileContainingPixel(size_t const indexRowPixel, size_t const indexColPixel)
{
	size_t th = getTileHeight();	
	size_t tw = getTileWidth();
	size_t indexRowGlobalTile = indexRowPixel/th;
	size_t indexColGlobalTile = indexColPixel/tw;
	return std::make_pair(indexRowGlobalTile, indexColGlobalTile);
}

std::shared_ptr<std::vector<uint32_t>> OmeTiffLoader::getBoundingBoxVirtualTileData(size_t const indexRowPixelMin, size_t const indexRowPixelMax,
                                                                    size_t const indexColPixelMin, size_t const indexColPixelMax)
{

	// Convention 
	// rows are X coordinate (increasing from top to bottom)
	// cols are Y coordinate (increasing from left to right)
	// we need to transform from Local Tile Coordinate to Global Pixel Coordiate to Virtual Tile Coordinate

	auto topLeftTile = getTileContainingPixel(indexRowPixelMin, indexColPixelMin);
	auto bottomRightTile = getTileContainingPixel(indexRowPixelMax, indexColPixelMax);
	auto minRowIndex = topLeftTile.first;
	auto minColIndex = topLeftTile.second;
	auto maxRowIndex = bottomRightTile.first;
	auto maxColIndex = bottomRightTile.second;

	// now loop through each tile, get tile data, fill the virtual tile vector

	auto tw = gsTiffTileLoader->tileWidth(0);
	auto th = gsTiffTileLoader->tileHeight(0);
	std::shared_ptr<std::vector<uint32_t>> tileData = std::make_shared<std::vector<uint32_t>>(tw * th);

	auto vtw = indexColPixelMax-indexColPixelMin+1;
	auto vth = indexRowPixelMax-indexRowPixelMin+1;
	std::shared_ptr<std::vector<uint32_t>> virtualTileData = std::make_shared<std::vector<uint32_t>>(vtw * vth);

	for (int i = minRowIndex; i <= maxRowIndex; ++i)
	{
		for (int j = minColIndex; j <= maxColIndex; ++j)
		{
			gsTiffTileLoader->loadTileFromFile(tileData, i, j, 0, 0);	
			// take row slice from local tile and place it in virtual tile
				size_t initialLocalX = indexRowPixelMin > i*th ? indexRowPixelMin-i*th : 0;	
				size_t endLocalX = indexRowPixelMax < (i+1)*th ? indexRowPixelMax-i*th: th;
				for (size_t localX=initialLocalX; localX<=endLocalX; ++localX){
					size_t globalX = i*th + localX;
					size_t virtualX = globalX - indexRowPixelMin;
					size_t initialLocalY = indexColPixelMin > j*tw ? indexColPixelMin-j*tw : 0;
					size_t endLocalY = indexColPixelMax < (j+1)*tw ? indexColPixelMax-j*tw : tw;
					size_t initialGLobalY = j*tw + initialLocalY;
					size_t initialVirtualY = initialGLobalY - indexColPixelMin;
					std::copy(tileData->begin()+localX*tw+initialLocalY, tileData->begin()+localX*tw+endLocalY+1,virtualTileData->begin()+virtualX*vtw+initialVirtualY);					
				}	
		}
	}
	return virtualTileData;
}
