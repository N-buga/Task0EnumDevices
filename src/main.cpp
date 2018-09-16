#include <CL/cl.h>
#include <libclew/ocl_init.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>


template <typename T>
std::string to_string(T value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

void reportError(cl_int err, const std::string &filename, int line)
{
    if (CL_SUCCESS == err)
        return;

    // Таблица с кодами ошибок:
    // libs/clew/CL/cl.h:103
    // P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
    std::string message = "OpenCL error code " + to_string(err) + " encountered at " + filename + ":" + to_string(line);
    throw std::runtime_error(message);
}

#define OCL_SAFE_CALL(expr) reportError(expr, __FILE__, __LINE__)


int main()
{
    // Пытаемся слинковаться с символами OpenCL API в runtime (через библиотеку libs/clew)
    if (!ocl_init())
        throw std::runtime_error("Can't init OpenCL driver!");

    // Откройте 
    // https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/
    // Нажмите слева: "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformIDs"
    // Прочитайте документацию clGetPlatformIDs и убедитесь что этот способ узнать сколько есть платформ соответствует документации:
    cl_uint platformsCount = 0;
    OCL_SAFE_CALL(clGetPlatformIDs(0, nullptr, &platformsCount));
    std::cout << "Number of OpenCL platforms: " << platformsCount << std::endl;

    // Тот же метод используется для того чтобы получить идентификаторы всех платформ - сверьтесь с документацией, что это сделано верно:
    std::vector<cl_platform_id> platforms(platformsCount);
    OCL_SAFE_CALL(clGetPlatformIDs(platformsCount, platforms.data(), nullptr));

    for (int platformIndex = 0; platformIndex < platformsCount; ++platformIndex) {
        std::cout << "Platform #" << (platformIndex + 1) << "/" << platformsCount << std::endl;
        cl_platform_id platform = platforms[platformIndex];

        // Откройте документацию по "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformInfo"
        // Не забывайте проверять коды ошибок с помощью макроса OCL_SAFE_CALL
        size_t platformNameSize = 0;
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &platformNameSize));
        // TODO 1.1
        // Попробуйте вместо CL_PLATFORM_NAME передать какое-нибудь случайное число - например 239
        // Т.к. это некорректный идентификатор параметра платформы - то метод вернет код ошибки
        // Макрос OCL_SAFE_CALL заметит это, и кинет ошибку с кодом
        // Откройте таблицу с кодами ошибок:
        // libs/clew/CL/cl.h:103
        // P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
        // Найдите там нужный код ошибки и ее название
        // Затем откройте документацию по clGetPlatformInfo и в секции Errors найдите ошибку, с которой столкнулись
        // в документации подробно объясняется, какой ситуации соответствует данная ошибка, и это позволит проверив код понять чем же вызвана данная ошибка (не корректным аргументом param_name)
        // Обратите внимание что в этом же libs/clew/CL/cl.h файле указаны всевоможные defines такие как CL_DEVICE_TYPE_GPU и т.п.

        // TODO 1.2
        // Аналогично тому как был запрошен список идентификаторов всех платформ - так и с названием платформы, теперь, когда известна длина названия - его можно запросить:
        std::vector<unsigned char> platformName(platformNameSize, 0);
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, platformNameSize, platformName.data(), nullptr));
        std::cout << "    Platform name: " << platformName.data() << std::endl;

        // TODO 1.3
        // Запросите и напечатайте так же в консоль вендора данной платформы
        size_t platformVendorNameSize = 0;
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 0, nullptr, &platformVendorNameSize));
        std::vector<unsigned char> platformVendorName(platformVendorNameSize, 0);
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, platformVendorNameSize, platformVendorName.data(), nullptr));
        std::cout << "    Platform vendor: " << platformVendorName.data() << std::endl;


        // TODO 2.1
        // Запросите число доступных устройств данной платформы (аналогично тому как это было сделано для запроса числа доступных платформ - см. секцию "OpenCL Runtime" -> "Query Devices")
        cl_uint devicesCount = 0;
        OCL_SAFE_CALL(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &devicesCount));
        std::cout << "Number of platform devices: " << devicesCount << std::endl;

        std::vector<cl_device_id> devices(devicesCount);
        OCL_SAFE_CALL(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, devicesCount, devices.data(), nullptr));

        for (int deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex) {
            // TODO 2.2
            // Запросите и напечатайте в консоль:
            // - Название устройства
            // - Тип устройства (видеокарта/процессор/что-то странное)
            // - Размер памяти устройства в мегабайтах
            // - Еще пару или более свойств устройства, которые вам покажутся наиболее интересными
            std::cout << "  Device #" << (deviceIndex + 1) << "/" << devicesCount << std::endl;
            cl_device_id device = devices[deviceIndex];

            size_t deviceNameSize = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_NAME, 0, nullptr, &deviceNameSize));
            std::vector<unsigned char> deviceName(deviceNameSize, 0);
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_NAME, deviceNameSize, deviceName.data(), nullptr));

            std::cout << "      Device name: " << deviceName.data() << std::endl;

            cl_device_type deviceType;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, nullptr));

            std::cout << "      Device type: ";

            switch(deviceType) {
                case CL_DEVICE_TYPE_CPU:
                    std::cout << "CPU" << std::endl;
                    break;
                case CL_DEVICE_TYPE_GPU:
                    std::cout << "GPU" << std::endl;
                    break;
                default:
                    std::cout << "Other" << std::endl;
            }

            cl_ulong memorySize = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &memorySize, nullptr));

            std::cout << "      Device global memory: " << (memorySize / (1024*1024)) << "Mb" << std::endl;

            size_t openCL_C_version_size = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, 0, nullptr,
                                          &openCL_C_version_size));

            std::vector<char> openCL_C_version(openCL_C_version_size, 0);
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, openCL_C_version_size, openCL_C_version.data(),
                                          nullptr));


            std::cout << "      Higher OpenCL C version that are supported by the compiler for device: " <<
                                                                                                         openCL_C_version.data() << std::endl;
            cl_uint max_work_item_dimensions = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &max_work_item_dimensions, nullptr));

            std::vector<size_t> dimension_sizes(max_work_item_dimensions);
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*max_work_item_dimensions, dimension_sizes.data(), nullptr));

            size_t  max_size = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &max_size, nullptr));

            std::cout << "      Maximum size for work group: " << max_size << std::endl;
            std::cout << "      Maximum size in each dimension: (";
            for (int dim_size = 0; dim_size < max_work_item_dimensions; dim_size++) {
                std::cout << dimension_sizes[dim_size];
                if (dim_size != max_work_item_dimensions - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << ")" << std::endl;

            cl_uint max_compute_units = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units), &max_compute_units,
                                          nullptr));
            std::cout << "      Maximum number of parallel compute units: " << max_compute_units << std::endl;

            cl_uint max_clock_freq = 0;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &max_clock_freq,
                                          nullptr));
            std::cout << "      Maximum configured clock frequency of the device: " << max_clock_freq << "MHz" << std::endl;

            cl_bool is_little_endian;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool), &is_little_endian, nullptr));
            std::cout << "      Is little endian device: ";
            switch(is_little_endian) {
                case CL_TRUE:
                    std::cout << "Yes" << std::endl;
                    break;
                case CL_FALSE:
                    std::cout << "No" << std::endl;
                    break;
                default:
                    std::cout << "Unknown" << std::endl;
            }

            cl_bool is_available;
            OCL_SAFE_CALL(clGetDeviceInfo(device, CL_DEVICE_AVAILABLE, sizeof(cl_bool), &is_available, nullptr));
            std::cout << "      Is available: ";
            switch(is_available) {
                case CL_TRUE:
                    std::cout << "Yes" << std::endl;
                    break;
                case CL_FALSE:
                    std::cout << "No" << std::endl;
                    break;
                default:
                std::cout << "Unknown" << std::endl;
            }

        }
    }

    return 0;
}