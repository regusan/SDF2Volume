# SDF2Volume
SDFをNxNxNのバイナリボリュームデータに変換する


## 特徴
SDFをNxNxNのボリュームデータに変換し、ファイル書き出しやアニメーション再生が行えます。

## データ形式
ヘッダー無しのNxNxNのchar型データ。(3x3x3のボリュームデータなら27Byte)

## スクリーンショット


## 依存ライブラリ
なし


## インストールの流れと実行例

### 1. リポジトリのクローン
```bash
git clone https://github.com/regusan/SDF2Volume.git
cd SDF2Volume
```

### 2. 依存ライブラリの準備

```bash
```

### 3. ビルド
```bash
make
```
### 4. 実行


```bash
./sdf2vol output/volumedata.dat
```
## 使用方法
```c++

/// @brief 単位球SDF
SDFPtr SphereSDF = [](float x, float y, float z) -> float
{
    return x * x + y * y + z * z - 0.5f;
};

/// @brief  Torus - exact SDF
SDFPtr TorusSDF = [](float x, float y, float z) -> float
{
    float R = 0.5f, r = 0.2f;
    float qx = std::sqrt(x * x + z * z) - R;
    return std::sqrt(qx * qx + y * y) - r;
};


Volume volume1(size);
Volume volume2(size);

//SDFモデルのセット
volume1.SetSDF(TorusSDF);
volume2.SetSDF(SphereSDF);

//ブーリアン演算。表面を定義するならmin(0)で負の値を0とする。
Volume finalVolume = (volume1.min(0) | volume2.min(0));

//ボリュームのサマリーを表示
std::cout << finalVolume;

//書き出し
std::ofstream file(argv[1], std::ios::binary);
(!finalVolume).ExportBinary(file);//書き出し時は正の値がボリュームとみなされるため反転
```