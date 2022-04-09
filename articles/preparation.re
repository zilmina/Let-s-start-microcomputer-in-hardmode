= マイコンの機能を使えるようにしよう

== 今回使用するマイコン
今回はRX631マイコンのR5F5631NDDFL（ROM容量384kB、RAM容量64kB、E2データFlash32kB、動作周波数（max）100MHz）を使用します。

== 環境構築
始めに開発環境を用意するところから始めましょう。いくつか注意点があるので丁寧に進めましょう。

=== PC側 
Renesasは公式の統合開発環境（IDE, Integrated Development Environment）として２種類をサポートしています。

 * e2studio
 * CubeSuite+

これらは元となったエディターの違いがあるものの機能的には大差ないです。今回はe2studioを使用して開発環境を整えます。
実機検証として使用したPCはSurface Pro3(OS:Windows10)です。

また、IDEにはプログラムの書き込みはできませんので別途「Renesas Flash Programmer」をインストールします。

==== e2studioのインストーラーの入手

 1. e2studioのインストールをRenesasのホームページから行います。（URL:@<href>{https://www.renesas.com/jp/ja/products/software-tools/tools/ide/e2studio.html}
//image[ide_e2studio][統合開発環境e2studioインストールページ][scale=0.5]{
//}
 2. インストーラをダウンロードをクリックします。
 3. 最新版のe2studioのバージョンを選択します。
 4. 該当バージョンに対する説明や注意事項の説明ページ下方にある「ダウンロード」をクリックします。
 5. My Renesasのログイン要求が表示されます。手順にしたがって登録を行いましょう。
 6. ログイン後自動的にダウンロードが行われます。

==== e2studioのインストール
インストーラにはネットワーク環境を必要とします。

 1. ダウンロードしたインストーラを実行します。
 2. デバイス・ファミリーに「RX」を選択します。
 3. 追加コンポーネントに「コラボレーション」「日本語言語パック」を選択します。
 4. 追加ソフトウェアは「GCC for RX」を選択します。GCC for RXではGNURXのユーザーアカウントを要求します。GNURXのアカウントを作成しましょう。
 5. 「インストール」を実行
 6. Renesas RX Compillerのインストーラーウィザードが立ち上がります。保存先はデフォルトのままインストールを行ってください。
 7. Renesas MCU Tools License Managerのインストールwizardが開きます。画面にしたがってインストールを行ってください。

=== マイコン側
マイコン側としては

 * マイコン
 * 基板
 * 書き込み装置
 * 電源

 が必要になります。

=== マイコンの入手
マイコンの入手先は基本的に通販経由での入手となります。
オススメとしては

 * @<href>{https://jp.rs-online.com/web/, RSオンライン}
 * @<href>{https://www.marutsu.co.jp/ , マルツオンライン}
 * @<href>{https://www.digikey.jp/, Digi-Key}
 * @<href>{https://www.chip1stop.com/, Chip One Stop}

 以上の４社です。購入するパーツの量、種類に応じて調べていただけると良いかと思います。

=== 基板の入手
マイコンが表面実装なのでユニバーサル基板では固定できません。そのため、専用の変換基板または既製品のマイコンボードをオススメします。
表面実装基板の半田付けはスルーホール基板の半田付けが十分できる方であれば多少の練習ないしは動画の解説を閲覧いただければ失敗しにくと思います。（URL:@<href>{https://www.youtube.com/watch?v=v9a0DA1o51E}）
変換基板の入手先は

 * 秋月電子通商（@<href>{http://akizukidenshi.com/catalog/g/gP-13520/, AE-QFP48PR5-DIP}）
 * amazon（@<href>{https://www.amazon.co.jp/uxcell-702105495698-QFN48ーDIP48アダプタプレート-PCBアダプタプレート-変換基板-10個入り/dp/B00NWFXJN0/ref=sr_1_7?__mk_ja_JP=カタカナ&keywords=0.5mm+48ぴん&qid=1554054088&s=gateway&sr=8-7, QFN48ーDIP48アダプタプレート}）


=== 書き込み装置の入手
マイコンの書き込みにはシリアルーUSB変換基板を使用します。今回は秋月電子で入手可能なＦＴ２３４Ｘ超小型ＵＳＢシリアル変換モジュール（URL:@<href>{http://akizukidenshi.com/catalog/g/gM-08461/}）を使用します。

=== 電源の入手
マイコンの消費電力は小さいので安定化電源は必要としないです。現実的には乾電池から得た６Vを三端子レギュレータ（TA48033S）で降圧して利用することをオススメします。

=== 推奨回路
今回の内容を実行するための検証用回路例はGitHub(https://github.com/zilmina/Let-s-start-microcomputer-in-hardmode-samples)にて共有いたします。

== e2studioでの新しいプロジェクトの作成
e2studioには各マイコンごとのプロジェクトファイル（各種変数ないしは関数を定義したファイル群）を生成することができます。

 1. e2studioを起動
 2. ファイル->新規->プロジェクトを選択
 3. C/C++->C/C++ projectを選択
 4. GCC for RX C/C++ Excutable projectを選択
 6. ターゲットデバイスを選択（記入欄右の...を選択->RX631->48pin->NDxFL）
 7. 終了を選択


