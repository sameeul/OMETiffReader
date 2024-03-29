#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "ome_tiff_loader.h"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<uint32_t>);

template <typename Sequence>
inline py::array_t<typename Sequence::value_type> as_pyarray_shared(std::shared_ptr<Sequence> seq_ptr) {
    auto size = seq_ptr->size();
    auto data = seq_ptr->data();
    auto capsule = py::capsule(new auto (seq_ptr), [](void *p) {delete reinterpret_cast<decltype(seq_ptr)*>(p);});
    return py::array(size, data, capsule);
 
}

template <typename Sequence>
inline py::array_t<typename Sequence::value_type> as_pyarray_shared_2d(std::shared_ptr<Sequence> seq_ptr, size_t num_rows, size_t num_cols) {
    auto size = seq_ptr->size();
    auto data = seq_ptr->data();
    auto capsule = py::capsule(new auto (seq_ptr), [](void *p) {delete reinterpret_cast<decltype(seq_ptr)*>(p);});
    return py::array(size, data, capsule).reshape({num_rows, num_cols});
 
}

PYBIND11_MODULE(image_loader_lib, m) {
  py::class_<OmeTiffLoader, std::shared_ptr<OmeTiffLoader>>(m, "OmeTiffLoader")
    .def(py::init<const std::string &>())
    .def("get_image_height", &OmeTiffLoader::getImageHeight)
    .def("get_image_width", &OmeTiffLoader::getImageWidth)
    .def("get_tile_height", &OmeTiffLoader::getTileHeight)
    .def("get_tile_width", &OmeTiffLoader::getTileWidth)
    .def("get_row_tile_count", &OmeTiffLoader::getRowTileCount)
    .def("get_column_tile_count", &OmeTiffLoader::getColumnTileCount)
    .def("get_tile_data",
        [](OmeTiffLoader& tl, size_t const indexGlobalTile) -> py::array_t<uint32_t> {
            auto tmp = tl.getTileData(indexGlobalTile);
            return as_pyarray_shared(tmp) ;
        }, py::return_value_policy::reference)
    .def("get_tile_data",
        [](OmeTiffLoader& tl, size_t const indexRowGlobalTile, size_t const indexColGlobalTile) -> py::array_t<uint32_t> {
            auto tmp = tl.getTileData(indexRowGlobalTile, indexColGlobalTile);
            return as_pyarray_shared(tmp);
        }, py::return_value_policy::reference)
    .def("get_tile_data_2d",
        [](OmeTiffLoader& tl, size_t const indexRowGlobalTile, size_t const indexColGlobalTile) -> py::array_t<uint32_t> {
            auto tmp = tl.getTileData(indexRowGlobalTile, indexColGlobalTile);
            return as_pyarray_shared_2d(tmp, tl.getTileHeight(), tl.getTileWidth()) ;;
        }, py::return_value_policy::reference)
     .def("get_tile_data_containing_pixel",
        [](OmeTiffLoader& tl, size_t const indexRowPixel, size_t const indexColPixel) -> py::array_t<uint32_t> {
            auto tmp = tl.getTileDataContainingPixel(indexRowPixel, indexColPixel);
            return as_pyarray_shared(tmp) ;
        }, py::return_value_policy::reference)    
    .def("get_virtual_tile_data_bounding_box",
        [](OmeTiffLoader& tl, size_t const indexRowMinPixel, size_t const indexRowMaxPixel, size_t const indexColMinPixel, size_t const indexColMaxPixel) -> py::array_t<uint32_t> {
            auto tmp = tl.getBoundingBoxVirtualTileData(indexRowMinPixel, indexRowMaxPixel, indexColMinPixel, indexColMaxPixel);
            return as_pyarray_shared(tmp) ;
        }, py::return_value_policy::reference)
        .def("get_virtual_tile_data_bounding_box_2d",
        [](OmeTiffLoader& tl, size_t const indexRowMinPixel, size_t const indexRowMaxPixel, size_t const indexColMinPixel, size_t const indexColMaxPixel) -> py::array_t<uint32_t> {
            auto tmp = tl.getBoundingBoxVirtualTileData(indexRowMinPixel, indexRowMaxPixel, indexColMinPixel, indexColMaxPixel);
            auto ih = tl.getImageHeight();
	        auto iw = tl.getImageWidth();
	        auto indexTrueRowPixelMax = indexRowMaxPixel > ih ? ih : indexRowMaxPixel;
	        auto indexTrueColPixelMax = indexColMaxPixel > iw ? iw : indexColMaxPixel;
            size_t num_rows = indexTrueRowPixelMax - indexRowMinPixel + 1;
            size_t num_cols = indexTrueColPixelMax - indexColMinPixel + 1;
            return as_pyarray_shared_2d(tmp, num_rows, num_cols) ;
        }, py::return_value_policy::reference)
        .def("get_virtual_tile_data_bounding_box_2d_strided",
        [](OmeTiffLoader& tl, size_t const indexRowMinPixel, size_t const indexRowMaxPixel, size_t const rowStride,  size_t const indexColMinPixel, size_t const indexColMaxPixel, size_t const colStride) -> py::array_t<uint32_t> {
            auto tmp = tl.getBoundingBoxVirtualTileDataStrideVersion(indexRowMinPixel, indexRowMaxPixel, rowStride, indexColMinPixel, indexColMaxPixel, colStride);
            auto ih = tl.getImageHeight();
	        auto iw = tl.getImageWidth();
	        auto indexTrueRowPixelMax = indexRowMaxPixel > ih ? ih : indexRowMaxPixel;
	        auto indexTrueColPixelMax = indexColMaxPixel > iw ? iw : indexColMaxPixel;
            size_t num_rows = (indexTrueRowPixelMax - indexRowMinPixel)/rowStride+1;
            size_t num_cols = (indexTrueColPixelMax - indexColMinPixel)/colStride+1;
            return as_pyarray_shared_2d(tmp, num_rows, num_cols) ;
        }, py::return_value_policy::reference);
}