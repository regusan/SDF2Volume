#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <algorithm>

using SDFPtr = float (*)(float, float, float);

class Volume
{
    // A+B=C
    using VolumeOperatorFunc = void (*)(const float &A, const float &B, float &C);

private:
    static void operator3Base(const Volume &A, const Volume &B, Volume &C, VolumeOperatorFunc func)
    {
        if (A.size != B.size || A.size != C.size)
        {
            std::cerr << "Invalid Size" << std::endl;
            return;
        }
        for (size_t i = 0; i < A.size; i++)
        {
            for (size_t j = 0; j < A.size; j++)
            {
                for (size_t k = 0; k < A.size; k++)
                {
                    func(A.data[i][j][k], B.data[i][j][k], C.data[i][j][k]);
                }
            }
        }
    }

public:
    size_t size;
    Volume(size_t size) : size(size), data(size, std::vector<std::vector<float>>(size, std::vector<float>(size, 0))) {}
    ~Volume();

    /// @brief SDF関数ポインタを適用する
    /// @param sdf 関数ポインタ
    /// @param valueScale 値のマルチプライヤー
    /// @param coordScale 座標のマルチプライヤー(デフォルト:-1~1)
    void SetSDF(SDFPtr sdf, float valueScale = 1.0f, float coordScale = 1.0f);

    /// @brief Z方向で2Dに切り出して2値化してプリント
    /// @param os
    /// @param depth Z方向インデックス
    /// @param threshold 2値化閾値
    /// @return
    std::ostream &Print(std::ostream &os, size_t depth = 0, float threshold = 0.0f);

    void ExportBinary(std::ofstream &file)
    {
        if (!file.is_open())
        {
            std::cerr << "File stream is not open for writing." << std::endl;
            return;
        }

        // データ本体を書き込み
        for (const auto &layer : data)
        {
            for (const auto &row : layer)
            {
                // float -> char変換用バッファ
                std::vector<char> charRow(row.size());
                for (size_t i = 0; i < row.size(); ++i)
                {
                    // 0.0f - 1.0f の範囲のfloatを 0 - 255 の範囲のcharに変換
                    charRow[i] = static_cast<char>(std::clamp(row[i] * 255.0f, 0.0f, 255.0f));
                }
                file.write(reinterpret_cast<const char *>(charRow.data()), charRow.size() * sizeof(char));
            }
        }

        file.close();
    }
    friend std::ostream &operator<<(std::ostream &os, Volume &volume)
    {
        size_t step = volume.size / 10 + 1;

        for (size_t i = 0; i < volume.data.size(); i += step)
        {
            for (size_t j = 0; j < volume.data[i].size(); j += step)
            {
                for (size_t k = 0; k < volume.data[i][j].size(); k += step)
                {
                    os << volume.data[i][j][k] << " ";
                }
                os << std::endl;
            }
            os << std::endl;
        }
        return os;
    }

    Volume min(const float &value)
    {
        Volume retval(size);
        for (size_t i = 0; i < this->size; i++)
        {
            for (size_t j = 0; j < this->size; j++)
            {
                for (size_t k = 0; k < this->size; k++)
                {
                    retval.data[i][j][k] = std::min(value, this->data[i][j][k]);
                }
            }
        }
        return retval;
    }
    Volume max(const float &value)
    {
        Volume retval(size);
        for (size_t i = 0; i < this->size; i++)
        {
            for (size_t j = 0; j < this->size; j++)
            {
                for (size_t k = 0; k < this->size; k++)
                {
                    retval.data[i][j][k] = std::max(value, this->data[i][j][k]);
                }
            }
        }
        return retval;
    }

    Volume operator|(Volume &other) const
    {
        Volume result(this->size);
        operator3Base(*this, other, result, [](const float &a, const float &b, float &c)
                      { c = a + b; });
        return result;
    }
    Volume operator|(Volume &&other) const
    {
        Volume result(this->size);
        operator3Base(*this, other, result, [](const float &a, const float &b, float &c)
                      { c = a + b; });
        return result;
    }

    Volume operator-(const Volume &other) const
    {
        Volume result(this->size);
        operator3Base(*this, other, result, [](const float &a, const float &b, float &c)
                      { c = a - b; });
        return result;
    }
    Volume operator-(const Volume &&other) const
    {
        Volume result(this->size);
        operator3Base(*this, other, result, [](const float &a, const float &b, float &c)
                      { c = a - b; });
        return result;
    }

    Volume operator&(const Volume &other) const
    {
        Volume result(this->size);
        operator3Base(*this, other, result, [](const float &a, const float &b, float &c)
                      { c = a * b; });
        return result;
    }
    Volume operator&(const Volume &&other) const
    {
        Volume result(this->size);
        operator3Base(*this, other, result, [](const float &a, const float &b, float &c)
                      { c = a * b; });
        return result;
    }

    Volume operator|=(const Volume &other)
    {
        operator3Base(*this, other, *this, [](const float &a, const float &b, float &c)
                      { c = a + b; });
        return *this;
    }

    Volume operator!=(const Volume &other)
    {
        operator3Base(*this, other, *this, [](const float &a, const float &b, float &c)
                      { c = a - b; });
        return *this;
    }

    Volume operator!()
    {
        operator3Base(*this, *this, *this, [](const float &a, const float &, float &c)
                      { c = -a; }); // bは使わない!
        return *this;
    }

private:
    std::vector<std::vector<std::vector<float>>> data;
};
