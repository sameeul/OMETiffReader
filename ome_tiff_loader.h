#include <string>
#include <tuple>
#include <memory>
#include <vector>
#include <fast_loader/specialised_tile_loader/grayscale_tiff_strip_loader.h>
#include <fast_loader/specialised_tile_loader/grayscale_tiff_tile_loader.h>
#include <omp.h>

class OmeTiffLoader{

    private:
        std::unique_ptr<fl::AbstractTileLoader<fl::DefaultView<uint32_t>>> gsTiffTileLoader;
        size_t nThreads = 1;
        std::string fNameWithPath = "";
        std::unique_ptr<std::vector<size_t>>  getImageDimensions(const std::string& filePath) const;
        std::unique_ptr<std::vector<size_t>>  calculateTileDimensions(const std::string& filePath) const;
	    bool checkTileStatus(const std::string& filePath) const;
        std::pair<size_t, size_t> getTileContainingPixel(size_t const indexRowPixel, size_t const indexColPixel);

    public:
        OmeTiffLoader(const std::string &fNameWithPath);
        ~OmeTiffLoader();
        std::shared_ptr<std::vector<uint32_t>> getTileDataContainingPixel(size_t const indexRowPixel, size_t const indexColPixel);
        std::shared_ptr<std::vector<uint32_t>> getTileDataBoundingBox(size_t const indexRowPixelMin, size_t const indexRowPixelMax,
                                                                    size_t const indexColPixelMin, size_t const indexColPixelMax);
        std::shared_ptr<std::vector<uint32_t>> getTileData(size_t const indexRowGlobalTile, size_t const indexColGlobalTile);
        std::shared_ptr<std::vector<uint32_t>> getTileData(size_t const indexGlobalTile);
        std::shared_ptr<std::vector<uint32_t>> getTileSectionData(size_t xRefCoord, size_t yRefCoord, int option);
        std::shared_ptr<std::vector<uint32_t>> getBoundingBoxVirtualTileData(size_t const indexRowPixelMin, size_t const indexRowPixelMax,
                                                                    size_t const indexColPixelMin, size_t const indexColPixelMax);
        std::shared_ptr<std::vector<uint32_t>> getBoundingBoxVirtualTileDataStrideVersion(size_t const indexRowPixelMin, size_t const indexRowPixelMax,
                                                                    size_t rowStride, size_t const indexColPixelMin, size_t const indexColPixelMax, 
                                                                    size_t colStride);
        size_t getRowTileCount () const;
        size_t getColumnTileCount () const;
        size_t getImageHeight() const ;
        size_t getImageWidth () const ;
        size_t getTileHeight () const ;
        size_t getTileWidth () const ;

};