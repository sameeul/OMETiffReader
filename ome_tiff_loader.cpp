
#include "ome_tiff_loader.h"

OmeTiffLoader::OmeTiffLoader(const std::string &fNameWithPath){
    if (checkTileStatus(fNameWithPath))
		{
			gsTiffTileLoader = std::make_unique<GrayscaleTiffTileLoader<uint32_t>>(nThreads, fNameWithPath);
		} 
		else 
		{
			auto imageDims = getImageDimensions(fNameWithPath); //vector of (tw, th, td)
			size_t defaultWidthSize = 1024;
			size_t defaultHeightSize = 1024;
			size_t defaultDepthSize = 1;
			size_t tw = std::min({imageDims->data()[0], defaultWidthSize});
			size_t th = std::min({imageDims->data()[1], defaultHeightSize});
			size_t td = std::min({imageDims->data()[2], defaultDepthSize});;
			// since file is not Tiled, we request a tile size
            gsTiffTileLoader = std::make_unique<GrayscaleTiffStripLoader<uint32_t>>(nThreads, fNameWithPath, tw, th, td);
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

std::unique_ptr<std::vector<size_t>>  OmeTiffLoader::getImageDimensions(const std::string& filePath)
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

bool OmeTiffLoader::checkTileStatus(const std::string& filePath)
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