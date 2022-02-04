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

PYBIND11_MODULE(image_loader_lib, m) {
  py::class_<OmeTiffLoader, std::shared_ptr<OmeTiffLoader>>(m, "OmeTiffLoader")
    .def(py::init<const std::string &>())
    .def("get_tile_data",
        [](OmeTiffLoader& tl, size_t const indexRowGlobalTile, size_t const indexColGlobalTile) -> py::array_t<uint32_t> {
            auto tmp = tl.getTileData(indexRowGlobalTile, indexColGlobalTile);
            return as_pyarray_shared(tmp) ;
        }, py::return_value_policy::reference);
}