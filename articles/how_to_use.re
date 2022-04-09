= マイコンの周辺機能を使ってみよう
マイコンの周辺機能はレジスタと呼ばれる高速に変更できる記憶領域に1と0を入力することによって設定変更を行います。レジスタのどのアドレスがどの機能に該当するのかはRenesas様のホームページにある「RX63N グループ、RX631 グループ ユーザーズマニュアル ハードウェア編」に記載されています。また、RX631マイコンはレジスタ変更の順番をかなり重要視します。このため動作実績をもつプログラムはかなり重要です。RX周辺クイックラーニングというページが存在しますので、適宜こちらも参照して進めましょう。


== レジスタの叩き方
レジスタはマイコンの記憶領域でe2studioによって生成された構造体によって操作し易く宣言されています。ただし、ハードウェアユーザーマニュアルにおけるアドレスの表記と一致しないこともあるので注意が必要です。

=== レジスタの説明
//image[register_difinition][レジスタの仕様][scale=0.5]{
//}
@<img>{register_difinition}はユーザーマニュアルハードウェア編のI/Oポートのレジスタの内、ポート方向レジスタの説明です。どのビットにどのようなデータを代入すればどのような機能が実現されるのか記載してあります。
用語の確認をしておきましょう。
//quote{
ビット　ー　コンピューターの中で扱うデータの最小単位。1ビットで2進数の1桁が0か1かを表せる。文字や画像など、すべての情報はビットの組み合わせで表現されている。ただし、実際にコンピューターが扱う情報の単位は8ビットを1組にしたバイト単位で表すことが多い。1バイト＝8ビットである。
@<br>{}
ASCII.jpデジタル用語辞典より
//}
プログラム上ではこれらのビットにデータを格納するには２つの方法があります。一つづつ代入するか、まとめて１度に代入するかです。

==== レジスタへの代入（一つづつ）
例としてI/OポートのP17端子を出力ポートとして機能するようレジスタに書き込みます。
//listnum[register_indivisual][レジスタへの代入を一つづつ行う場合]{
	PORT1.PDR.BIT.B7 = 1;	//P17を出力と定義
//}
レジスタへの代入をビット単位で行う場合は@<code>{src/iodefine.h}で宣言されている構造体を使用します。"."はドット演算子ないしはメンバ参照演算子と呼ばれます。
レジスタにビット単位で書き込むことには１ビットで操作で機能をON,OFFできる場合、プログラムでした操作が分かり易くなります。ただし、まとめて書き込む場合と異なり操作の回数が増えることになりプロテクト解除と相性が悪い場合があります。レジスタに対する書き込みが正常に行えない場合は@<hd>{regiter_word}の操作を行いましょう。

===={regiter_word} レジスタへの代入（まとめて）
例として、
//listnum[register_word_code][レジスタへの代入をまとめておこなう場合]{
SYSTEM.PLLCR.WORD = 0x1300; /*PLL逓倍×20 入力1分周 (10MHz*20=200MHz)*/
//}
複数のビットにデータを書き込む際は１０進法ないしは１６進法でビットの内容を記述します。二進数で表記しているものは0b、１６進数は0x、特に指定なき場合は１０進数で表記しているものとします。今回例に挙げたレジスタはb1-b0がPLL入力分周比選択ビット、b13-b8周波数逓倍率設定ビットの該当し、それぞれ１分周（0b00）、２０逓倍（0b010011）を指定します。この場合PLLコントロールレジスタに代入すべき値は0b 00 010011 000000 00、すなわち0x1300と表記します。

レジスタ操作を短く表現できるので複雑なレジスタ操作を記述する場合やレジスタ操作が冗長になってしまう場合には便利な操作です。しかし、一般的にレジスタの名称と値の関係がわかりにくくなるため可読性が低下します。コメントで操作した内容を記述するなどの工夫をオススメします。

== 汎用出力ポート
汎用出力ポートはマイコンの該当するPINの電圧をON、OFFする機能です。マイコンでLEDを光らせたりモータードライバーの操作をするための基本となる機能です。@<br>{}
マイコンのどのピンで汎用ポートが使えるかはユーザーマニュアルハードウェア編の機能別端子一覧に記してあります。例として、48ピンLQFPのピン機能を確認します。
//image[pin_func][ピン機能（P14）の例]{
機能別端子一覧16番ピンの内容
//}
複数の機能が一つの端子に割り当てられています。実際にマイコンを使用した回路を製作するときはどの機能をどのピンで使用するか取捨選択しながら設計を行います。

ユーザーマニュアルのI/Oポートの章を閲覧しながら進めます。汎用ポートの出力のために操作するレジスタは主に２種類です。

 * ポート方向レジスタ
 * ポート出力データレジスタ
 * ポート入力レジスタ

I/Oポートは入力、出力両方扱うことができますが同時に取り扱うことはできません。なので初期化としてポートの入出力を宣言する必要があります。ポートが出力設定の場合、初期化後はポート出力データレジスタに格納した値に対応してHigh(3.3V)もしくLow(0V)が出力されます。

//listnum[io-code][汎用ポートのソースコード]{
void main(void){
    PORT1.PDR.BIT.B4 = 1;	//P14を出力と定義
	PORT3.PDR.BIT.B5 = 0;	//P35を入力と定義
	while(1){
		PORT1.PODR.BIT.B4 = PORT3.PIDR.BIT.B5;
	}
}
//}

マイコンで動かしてみるとボタンを押した時だけLEDが点灯するはずです。しかし、これでは時間変化のあるプログラミングができません。なので一定時間まで待機したり、一定時間ごとに関数を呼び出す事ができるようにしましょう。

===[column] Tips : Lチカデバッグ

　レジスタまわりの設定は順調に進めば記憶にすら残らないものですが大抵はそうもいかないのが実状です。原因はデバッガを使用していない場合レジスタ内部の状態が観測不能となるからです。そのためマイコンの配線が完了後にプログラムを実行してみたが動作しない場合、デバッグ用に汎用ポートについたLEDを用意しておくことをオススメします。@<br>{}
　プログラムが動かない、特にレジスタ操作後がおかしい場合はそもそもレジスタの書き換えが行われていない可能性があります。ですのでレジスタのBITを汎用出力レジスタに代入すればLEDで1、0が表示可能となります。ここからレジスタの書き換えが行われているかを一つずつ確認すれば消去法で問題の洗い出しができます。

===[/column]

== コンペアマッチタイマー
コンペアマッチタイマーは主にタイマー割り込みという定期的に関数を呼び出す機能を使うときに使います。
そのためには外部に取り付けた水晶発信子からでたクロックをコンペアマッチタイマーの属するモジュールまで伝達する必要があります。マイコンのメインクロックから各機能モジュールに伝達されるクロック信号の設定を行いましょう。

=== メインクロックの設定
//listnum[clock-code][クロック設定のソースコード]{
///-クロック初期化関数--------------------------------------------------
void main_clock_initialize(void){

	//PLL(Phase Locked Loop:発振器からの周波数を逓倍)の動作周波数はMAX:200MHz
	//動作周期は100MHz
	/* PLL回路 20逓倍× 入力1分周 (10.000MHz * 20 = 200MHz)
	 * 0b00 010011 000000 00 = 0x1300*/
	SYSTEM.PRCR.WORD = 0xa50b; /*クロックソース選択の保護の解除*/
	SYSTEM.PLLCR.WORD = 0x1300;
	SYSTEM.PLLCR2.BIT.PLLEN = 0; /*PLL ENABLE */

	/* BCLK端子出力1/4(MAX:50MHz)(BCLKとBCLK端子出力は別)0010b
	 * PCLKB1/4(MAX:50MHz)0010b
	 * PCLKA=ICLK1/2(MAX:100MHz)0001b
	 * SDCLK停止 1
	 * BCLK停止 1
	 * ICK1/2(MAX:100MHz) 0001b
	 * FCK1/4(MAX:50MHz) 0010b
	 * 0b 0010 0001 1 1 00 0001 0010 0010 0001 0001 = 0x21C12211 */
	SYSTEM.SCKCR.LONG = 0x21C12211;

	/* IEBCK1/4(MAX:50MHz)0010b
	 * UCLK1/4(使わない場合0001b)
	 * 00000000 0001 0010b = 0x0012  */
	SYSTEM.SCKCR2.WORD = 0x0012;

	/*BCLK = (1/2) * BCLK(MAX:100MHz) */
	SYSTEM.BCKCR.BYTE = 0x01;

	/* クロックソース選択：PLL回路選択100
	 * 00000 100 00000000b = 0x0400*/
	SYSTEM.SCKCR3.WORD = 0x0400;

	SYSTEM.PRCR.WORD = 0xa500;	/*クロックソース選択の保護*/
}
//}
今回の検証用回路およびマイコンの周波数的仕様を確認しましょう。

//table[table_clk][検証用回路およびマイコンの周波数的仕様]{
機能	周波数
----------------------
外部水晶発振子（メインクロック）	10MHz
ICLK	100MHz (max)
PCLKA	100MHz (max)
PCLKB	50MHz (max)
FCLK	4MHz ~ 50MHz(ROM、E2 データフラッシュ P/E 時)
.    	50MHz (max)(E2データフラッシュ読み出し時)
BCLK	100MHz(max)
BCLK端子出力	50MHz (max)
SDCLK端子出力	50MHz (max)
PLL回路入力周波数	4MHz~16MHz
PLL回路VCO発振周波数	104MHz~200MHz
//}

メインクロックの設定として重要なことがあります。

 * USB機能を使用するかどうか？
 * マイコンで実現したい周波数は何か？

これらの内USBを使用するかどうかはかなり影響力が大きいです。なぜならUSBクロックは48MHzを必要とするため水晶発振子を3の倍数を含んだものに前もってしておかないといけないからです。

そして、今回はUSB機能は使わない想定で設定を行います。各クロックの周波数を上限値で設定を行います。（@<table>{table_clk_def}）マイコンのCPUを可能な限り早く動作させたいからです。
//table[table_clk_def][今回のマイコンにおける周波数設定]{
機能	周波数
----------------------
外部水晶発振子（メインクロック）	10MHz
ICLK	100MHz 
PCLKA	100MHz
PCLKB	50MHz 
FCLK	50MHz
BCLK	停止
SDCLK端子出力	停止
PLL回路入力周波数	10MHz
PLL回路VCO発振周波数	200MHz
//}

では、レジスタ操作の説明に移ります。

==== SYSTEM.PRCR レジスタ
ユーザーマニュアルのレジスタライトプロテクション機能に該当するレジスタです。クロック発生回路関連レジスタには書き換えを禁じるプロテクトがかかっています。クロック関連レジスタを書き換えるときにはb0プロテクトビット０を（書き込み許可）に変更してから操作しましょう。
//listnum[prcr_code][クロック関連レジスタの書き換え]{
	SYSTEM.PRCR.WORD = 0xa501;	/*クロックソース選択の保護*/
	SYSTEM.PRCR.WORD = 0xa500;	/*クロックソース選択の保護*/
//}
ただし、@<list>{failre_prcr_code}
//listnum[failre_prcr_code][プロテクト解除に失敗する一例]{
	SYSTEM.PRCR.BIT.PRCO = 1;	/*クロックソース選択の保護*/
	SYSTEM.PRCR.BIT.PRCO = 0;	/*クロックソース選択の保護*/
//}
理由としてはプロテクトレジスタはプロテクトビットの変更時にはPRCキーコードビットの書込みを同時に要求するからです。

==== SYSTEM.PLLCR
このレジスタはPLL回路に入力するクロックの周波数を調節する設定とPLL回路で周波数を何倍するかを設定します。
今回、水晶発振子の周波数は10MHzでPLL回路に入力周波数の範囲内なので１分周（0b00）、出力はPLL回路の上限周波数の200MHzにするため２０逓倍（0b010011）に設定します。

==== SYSTEM.PLLCR2
PLLの動作/停止を操作します。PLL回路の設定完了後に動作するように変更します。該当レジスタにはPLLEN（PLL Enable）ビットが存在し、このビットに０を書込みます。

==== SYSTEM.SCKCR
こちらはシステムクロックコントロールレジスタです。これで各種システムクロックを設定します。このレジスタにおいてクロックソースはPLL回路（出力200MHz）です。自分の設定したい周波数になるように設定します。

	 * BCLK端子出力1/4(MAX:50MHz)(BCLKとBCLK端子出力は別)0010b
	 * PCLKB1/4(MAX:50MHz)0010b
	 * PCLKA=ICLK1/2(MAX:100MHz)0001b
	 * SDCLK停止 1
	 * BCLK停止 1
	 * ICK1/2(MAX:100MHz) 0001b
	 * FCK1/4(MAX:50MHz) 0010b

==== SYSTEM.SCKCR2
IEBCK、UCK（USBクロック）の設定です。こちらも設定したい周波数に合わせて設定を行います。ただし、USBを使用しない場合は0b0001を設定します。

 * IEBCK 1/4 0b0010
 * UCK 使用しない　0b0001

==== SYSTEM.BCKCR
外部バスの端子出力の周波数を設定します。BCLKの周波数に依存しているので注意しましょう。

 * BCLK端子出力 50MHz : 1

==== SYSTEM.SCKCR3
システムクロックのソースクロックを選択します。今回はPLL回路からクロックを供給してもらいます。

 * クロックソース選択　PLL回路 : 0b100

=== メインクロックを踏まえたコンペアマッチタイマの設定
クロックの設定が完了したのでコンペアマッチタイマが使用可能なクロックの周波数が確定しました。
コンペアマッチタイマーに供給されるクロックはPCLK"B"です。ユーザーマニュアルにはPCLKと記載されていますが、クロック発生回路の章にて「周辺モジュールクロックPCLKA ETHERC,EDMACへ」「周辺モジュールクロック（PCLKB） ETHERC,EDMAC以外の周辺モジュールクロックへ」と記載されています。

今回はPCLKB=50MHzと設定しましたのでその前提で進みましょう。

//listnum[cmt_code][CMTのサンプルコード（1kHz）]{
void CMT0_initialize(void) {
	SYSTEM.PRCR.WORD = 0xa50b;	/*クロックソース選択の保護の解除*/
	MSTP(CMT0) = 0;				//CMT0の運用開始
	SYSTEM.PRCR.WORD = 0xa500;	/*クロックソース選択の保護*/

	CMT.CMSTR0.BIT.STR0 = 0;	// Disable CMT0 count
	CMT0.CMCR.BIT.CMIE = 0;		//CMI0(コンペアマッチ割り込み)禁止

	CMT0.CMCR.BIT.CKS = 3;		//512分周
	CMT0.CMCNT = 0;				//カウンタのリセット
	CMT0.CMCOR = 65536 - 1; 	//50MHz/512/65536 = 1Hz

	CMT0.CMCR.BIT.CMIE = 1;		//CMI0(コンペアマッチ割り込み)許可
	CMT.CMSTR0.BIT.STR0 = 1;	// Enable CMT0 count

	IEN(CMT0,CMI0)= 1;			//割り込みを許可する
	IPR(CMT0,CMI0)= 6;			//CMI0の割り込み優先度6を設定(MAX15,MIN1)
}
//}

//listnum[cmi_code][generate/inthandler.c内の改変部分]{
	#include "iodefine.h"
	(省略)
	void INT_Excep_CMT0_CMI0(void) {
		if (PORT1.PODR.BIT.B4 == 1){
			PORT1.PODR.BIT.B4 = 0;
		} else {
			PORT1.PODR.BIT.B4 = 1;
		}
	}
//}

//image[CMT][コンペアマッチとCMT0.CMCNTの値の関係]{
//}

コンペアマッチタイマーには大きく３つの機能をもっています

 * CMCNTレジスタをクロックごとにカウントアップする
 * CMCORレジスタとCMCNTレジスタが同じ値（コンペアマッチ）したときに割り込み要求が可能
 * コンペアマッチ時にCMCNTレジスタの値を0とする（カウンタクリア）

コンペアマッチレジスタのイメージとしては@<img>{CMT}となります。

==== コンペアマッチタイマの周期
コンペアマッチタイマの周期は以下の要素によって決定されます。

 * PCLKBの周波数
 * プリスケーラ（分周器）の値（CMT0.CMCR.BIT.CKS）
 * カウントの最大値（CMT0.CMCOR）

カウントの最大値は最大で65535です。（レジスタの大きさが１６ビット）また、プリスケーラの最大分周比は512分周です。結果今回の設定では50MHz/512/65536=1Hzが最も遅い周期となります。

==== マクロ
e2studioが生成したプログラムには一部マクロが含まれています。今回は

	 * MSTP()：モジュールの動作/停止の切り替え
	 * IEN()：割り込みの許可/禁止
	 * IPR()：割り込みの優先度設定

を使用しています。

==== MSTP
RX631マイコンには様々なモジュール（機能）が組み込まれているのですが消費電力の削減のためデフォルトでは機能が停止されています。なので機能を動作するように変更しなければなりません。ただし、例によってレジスタプロテクションがかかっていますのでプロテクト解除操作（SYSTEM.PRCR）が必須です。

==== IEN
割り込み機能を有効にします。割り込みが有効になるとgenerate/inthandler.c内の該当する関数が割り込み関数として呼び出されます。（今回はINT_Excep_CMT0_CMI0()）

==== IPR
割り込み関数の優先度を決定します。優先レベルは0~15まであり、数字が大きいほど優先されます。割り込み関数実行中に割り込み要因が起こる場合もあるので優先順位を決めておかないといけないわけです。

==== CMT.CMSTR0
コンペアマッチスタートレジスタです。コンペアマッチタイマーのカウント、つまりCMT0.CMCNTのカウントを行うかどうかを決定します。
//image[cmstr][CMSTR0レジスタ仕様]{
//}

==== CMT0.CMCR
コンペアマッチタイマーコントロールレジスタです。ここでは割り込みの許可/禁止の設定とクロック（PCLKB）を何分周して利用するかを設定します。
//table[cmcr][CMCRレジスタ仕様]{
ビット	シンボル	ビット名	機能	R/W
-------------------------------------
b1-b0	CKS[1:0]	クロック選択ビット	@<br>{} b1 b0 @<br>{} 0 0:PCLK/8 @<br>{} @<br>{} 0 1:PCLK/32 @<br>{} 1 0:PCLK/128 @<br>{}  1 1:PCLK/512	R/W
b5-b2	—	予約ビット	@<br>{} 読むと “0” が読めます。 @<br>{} 書く場合、“0” としてください	R/W
b6	CMIE	コンペアマッチ割り込み @<br>{} 許可ビット	@<br>{} 0:コンペアマッチ割り込み(CMIn)を禁止 @<br>{} 1:コンペアマッチ割り込み(CMIn)を許可	R/W
b7	—	予約ビット	@<br>{} 読んだ場合、その値は不定。 @<br>{} 書く場合、“1” としてください	R/W
b15-b8	—	予約ビット	@<br>{} 読むと “0” が読めます。 @<br>{} 書く場合、“0” としてください	R/W
//}
==== CMT0.CMCNT
コンペアマッチタイマカウンタです。１クロックごとにカウントアップします。コンペアマッチタイマーを開始させる場合はこのレジスタに０を代入してす初期化することを推奨します。

==== CMT0.CMCOR
コンペアマッチタイマコンスタントレジスタです。ここに設定した値に@<tt>{CMT0.CMCNT}がなったときコンペアマッチが起こります。注意事項としてこのレジスタは16ビットなので０〜65535の値の範囲までしか保持できません。これより大きな値を設定したい、つまり周期を遅くしたい場合はクロックの分周率の変更をしましょう。

== PWM
PWMとはPalse Wave Modulationの略でその名の通りPINをONにしている幅を変調することによって出力を変化させる方法です。実態は汎用出力とコンペアマッチタイマーの組み合わせで実現されています。

PWMはMTU（マルチファンクションタイマパルスユニット）の一部機能です。　MTU機能の出力ピンは複数ありますがMTUだけでなく、他の周辺機能も出力ピンを複数もっています。なので物理的に存在するピンより周辺機能の入出力の方が多い状況となっています。そのため、MPC（マルチファンクションピンコントローラ）を用いて使用したい端子に周辺機能の入出力を割り当てる必要があります。
以降はサンプルコードを用いて説明しましょう。
//listnum[pwm_code][PWMサンプルコード]{
void pwm_initialize(void) {
	int period = 500;				//分解能を500に設定
	int duty = 10;					//dyty比10/500=2%に設定

	// MTU3
	SYSTEM.PRCR.WORD = 0xa50b;		// Release Protect
	MSTP(MTU3) = 0;					// Wake up MTU3
	SYSTEM.PRCR.WORD = 0xa500;		// Protect

	// Set MPC
	PORT1.PMR.BIT.B4 = 1;			// Set P14: Peripheral

	MPC.PWPR.BIT.B0WI = 0;			// Release protect
	MPC.PWPR.BIT.PFSWE = 1;			// Release Protect

	MPC.P14PFS.BIT.PSEL = 1;		// Set P14: MTIOC3A

	MPC.PWPR.BIT.PFSWE = 0;			// Protect
	MPC.PWPR.BIT.B0WI = 1;			// Protect

	// PWM Settings
	MTU3.TCR.BIT.TPSC = 0;			// PCLK/1 = 50MHz/1 = 50MHz
	MTU3.TCR.BIT.CKEG = 0;			// Count rising edge
	MTU3.TCR.BIT.CCLR = 1;			// TGRAのコンペアマッチでMTU3のカウントをリセット

	MTU3.TMDR.BIT.MD = 2;			// MTU3 PWMモード1

	MTU3.TIORH.BIT.IOA = 6;			// Compare Output High
	MTU3.TIORH.BIT.IOB = 7;			// Compare Output Low

	MTU3.TGRA = period - 1;				// 1kHz(65535)
	MTU3.TGRB = 1;						// Duty 0%

	MTU3.TCNT = 0;						// Clear MTU3 count
	MTU.TSTR.BIT.CST3 = 1;				// Start MTU3 count

	MTU3.TGRB = duty;
}
//}

=== 端子と機能の割り当て

==== PORTm.PMR
ポートモードレジスタです。これは端子を汎用ポートとして使うか周辺機能（ペリフェラル）として使うかを設定します。この設定を失念してしまうと周辺機能出力は機能しません。このレジスタのデフォルトは汎用入出力ポートとして設定されているからです。

==== MPC.PWPR
MPCレジスタの書込みプロテクトレジスタです。このレジスタは二重にプロテクトがかかっておりPFSWEビット書込み禁止ビットを解除してからPFSレジスタ書き込み許可ビットを変更しなければなりません。

==== MPC.PmnPFS
端子機能制御レジスタです。ユーザーマニュアルに端子と周辺機能の対応表が記載されているのでこちらに合わせて設定します。P14端子でMTIOC3Aを使いたいので@<tt>{MPC.P14PFS.BIT.PSEL=1}となります。
//image[psel][64 ピン LQFP、48 ピン LQFP 端子入出力機能レジスタ設定]{
//}

=== PWM設定
ユーザーマニュアルにおける「PWMモード設定の手順」にしたがって設定を行います。

==== MTU3.TCR
タイマコントロールレジスタです。こちらのレジスタでは、分周比の選択、クロックエッジの選択、カウンタクリアの要因選択ができます。

//table[mtu_cmcr][CMCRレジスタ仕様]{
ビット	シンボル	ビット名	機能	R/W
--------------------------------------
b2-b0	TPSC[2:0]	タイマプリスケーラ選択ビット	@<img>{tpsc}を参照してください	R/W
b4-b3	CKEG[1:0]	クロックエッジ選択ビット	 @<br>{} b4 b3 @<br>{}  @<br>{} 0 0:立ち上がりエッジでカウント  @<br>{} 0 1:立ち下がりエッジでカウント  @<br>{} 1 x:両エッジでカウント	R/W
b7-b5	CCLR[2:0]	カウンタクリアビット	@<img>{cclr}を参照してください	R/W
//}
//image[tpsc][TPSCレジスタ仕様]{
//}
//image[cclr][CCLRレジスタ仕様]{
//}

==== TPU3.TMDR
タイマーモードレジスタです。このMTUには複数の動作モードが存在します。ここからPWMモード１を選択します。

PWMモード１はPWM出力を出す機能の内、MTUのTGRレジスタを２つペアにしてPWM波形を生み出すモードです。

//table[tmdr][TMDRRレジスタ仕様]{
ビット	シンボル	ビット名	機能	R/W
--------------------------------------
b3-b0	MD[3:0]	モード選択ビット	タイマの動作モードを設定します。 @<br>{} @<table>{md}を参照してください	R/W
b4	BFA	バッファ動作 A ビット	@<br>{} 0:TGRA と TGRC レジスタは通常動作 @<br>{} 1:TGRA と TGRC レジスタはバッファ動作	R/W
b5	BFB	バッファ動作 B ビット	@<br>{} 0:TGRB と TGRD レジスタは通常動作 @<br>{} 1:TGRB と TGRD レジスタはバッファ動作	R/W
b6	BFE	バッファ動作 E ビット	@<br>{} 0:MTU0.TGRE と MTU0.TGRF は通常動作 @<br>{} 1:MTU0.TGRE と MTU0.TGRF はバッファ動作	R/W
b7	—	予約ビット	読むと “0” が読めます。@<br>{} 書く場合、“0” としてください	R/W
//}

//table[md][MDビット仕様]{
ビット3	ビット2	ビット1	ビット0	説明
--------------------------------------
0	0	0	0	ノーマルモード
0	0	0	1	設定しないでください
0	0	1	0	PWM モード 1
0	0	1	1	PWM モード 2
0	1	0	0	位相計数モード 1
0	1	0	1	位相計数モード 2
0	1	1	0	位相計数モード 3
0	1	1	1	位相計数モード 4
1	0	0	0	リセット同期 PWM モード
1	0	0	1	設定しないでください。
1	0	1	x	設定しないでください。
1	1	0	0	設定しないでください。
1	1	0	1	相補 PWM モード 1(山で転送)
1	1	1	0	相補 PWM モード 2(谷で転送)
1	1	1	1	相補 PWM モード 3(山・谷で転送)
//}

==== MTU3.TIORH
タイマI/Oコントロールレジスタです。MTIOCx端子の機能を定義するレジスタです。
コンペアマッチ発生時に端子出力をどのように変更するかを定義します。
今回は(MTU3.TGRB/MTU3.TGRA)がデューティ比になるように設定するためにTGRAのコンペアマッチ時は出力をHigh、TGRBのコンペアマッチ時はトグル（出力の反転）とします。（@<img>{pwm_mode1_67}）

@<img>{pwm_mode1_67}はつまりカウンタクリアからMTU3.TGRBのコンペアマッチまで出力をHIGHにするということです。
//image[pwm_mode1_67][MTU3.TIORH.BIT.IOA = 6,MTU3.TIORH.BIT.IOB = 7設定時のPWM出力波形]{
//}
//table[tiorh_def][TIORHレジスタ仕様]{
ビット	シンボル	ビット名	機能	R/W
--------------------------------------
b3-b0	IOA[3:0]	I/O コントロール A ビット	MTU3.TIORH:@<table>{tiorh_ioa}	R/W
b7-b4	IOB[3:0]	I/O コントロール B ビット	MTU3.TIORH:@<table>{tiorh_iob}	R/W
//}
//table[tiorh_ioa][MTU3.TIORH.IOAレジスタ仕様]{
IOA3	IOA2	IOA1	IOA0	MTU3.TGRA の機能	MTIOC3A 端子の機能
0	0	0	0	@<br>{} アウトプットコンペア @<br>{} レジスタ	出力禁止
0	0	0	1	.	@<br>{} 初期出力は Low 出力 @<br>{} コンペアマッチで Low 出力
0	0	1	0	.	@<br>{} 初期出力は Low 出力 @<br>{} コンペアマッチで High 出力
0	0	1	1	.	@<br>{} 初期出力は Low 出力 @<br>{} コンペアマッチでトグル出力
0	1	0	0	.	出力禁止
0	1	0	1	.	@<br>{} 初期出力は High 出力 @<br>{} コンペアマッチで Low 出力
0	1	1	0	.	@<br>{} 初期出力は High 出力 @<br>{} コンペアマッチで High 出力
0	1	1	1	.	@<br>{} 初期出力は High 出力 @<br>{} コンペアマッチでトグル出力
1	x	0	0	インプットキャプチャ @<br>{} レジスタ	立ち上がりエッジでインプットキャプチャ
1	x	0	1	.	立ち下がりエッジで @<br>{} インプットキャプチャ
1	x	1	x	.	両エッジで @<br>{} インプットキャプチャ
//}
//table[tiorh_iob][MTU3.TIORH.IOBレジスタ仕様]{
IOB3	IOB2	IOB1	IOB0	MTU3.TGRB の機能	MTIOC3B 端子の機能
0	0	0	0	@<br>{} アウトプットコンペア @<br>{} レジスタ	出力禁止
0	0	0	1	.	@<br>{} 初期出力は Low 出力 @<br>{} コンペアマッチで Low 出力
0	0	1	0	.	@<br>{} 初期出力は Low 出力 @<br>{} コンペアマッチで High 出力
0	0	1	1	.	@<br>{} 初期出力は Low 出力 @<br>{} コンペアマッチでトグル出力
0	1	0	0	.	出力禁止
0	1	0	1	.	@<br>{} 初期出力は High 出力 @<br>{} コンペアマッチで Low 出力
0	1	1	0	.	@<br>{} 初期出力は High 出力 @<br>{} コンペアマッチで High 出力
0	1	1	1	.	@<br>{} 初期出力は High 出力 @<br>{} コンペアマッチでトグル出力
1	x	0	0	インプットキャプチャ @<br>{} レジスタ	立ち上がりエッジでインプットキャプチャ
1	x	0	1	.	立ち下がりエッジで @<br>{} インプットキャプチャ
1	x	1	x	.	両エッジで @<br>{} インプットキャプチャ
//}

==== MTU3.TGR,MTU3.TCNT
タイマジェネラルレジスタとタイマカウントレジスタです。

 * タイマカウントレジスタ
	** クロックをチェックエッジビットの設定にしたがってカウントアップします。16ビットの容量です。@<br>{} カウントクリアはカウンタクリアビット（CCLRビット）の設定に応じて発生します。
 * タイマジェネラルレジスタ
	 ** アウトプットコンペア/インプットキャプチャ兼用レジスタです。16ビットの容量があります。今回の使用用途としてはPWMの周期を設定、デューティー比の設定を行います。

== SCI
文字列をPCに対して送信するためにSCI（シリアル通信）機能を実装します。

仕様は以下の通りとします。

 * シリアル通信方式：調歩同期式
 * ボーレート（通信速度）：115200bps
 * 入出力端子：P26,P30
 * 使用するモジュール：SCI1

今回は受信側にTera Termというソフトを使用します。インストールと使用方法は@<hd>{teraterm|install}に記載します。


=== データ送信の手順
SCI通信のデータの流れは主に２つに分割されています。

 * TXI割り込み関数内
 * SCI＿print関数内

//image[sci_scheme][SCI通信データ送信時の流れ]{
//}

==== SCI_print関数内
SCI_printをデータを送信するときに呼び出す関数とします。引数はchar型の配列のポインタとします。この関数の役目は主に二つになります。

 * 送信バッファに受け取った文字列を追加する
 * TXI割り込みが許可されていない場合、割り込みを許可する

一つ目の項目を理解するには文字列の実態を認識する必要があります。計算機上では1と０しかもしくはその集合でのみ記録できます。なので1byte(=8bit)で表現できる数字と文字の対応を規定したものがああります。このルールを文字コードと呼びます。そのうちASCIIコードはc言語においてchar型で容易に扱える利点があります。文字コードで表現された文字は連なれば文字列となり@<tt>{"abc"}と表現すれば実態としては@<tt>{char c[4]={0x61,0x62,0x63,0x00\} ;}と記述されています。最後に何か足されていますがこれは「ヌル文字」といい文字列の終わりを意味しています。配列を読みだしたときにヌル文字が出てくれば文字列が終わったと理解することができます。

二つ目のTXI割り込み（TDRレジスタ送信後に起こる割り込み）許可を与える理由は文字列を一気に送信してもらうためです。送信するデータは送信バッファに記録しましたがこれだけでは送信してもらえません。TDRレジスタに送信内容を１度だけ書き込む必要があります。TDRレジスタの内容を送信した後に新しいデータを逐次送信バッファから書き出す必要があるのです。そのため、TDRレジスタのデータ送信を完了後に呼び出される割り込み関数で新しい送信内容を送信バッファから書き込んでもらうことによって自動的に処理を行います。

==== TXI関数内
ここでは未送信データがある場合はTDRレジスタに新しい内容を書き込みTXI割り込みを許可します。メイン関数内で待機せずに処理を行うのでリアルタイム性をあまり損なわずに処理が可能となります。

また、送信するものがない場合は割り込みを禁止して次のデータ送信まで待機します。

//listnum[sci_main][SCIサンプルコードのメイン関数][c]{
int main(void){
	main_clock_initialize();
	SCI_init(115200);
	SCI_println("Hello,world")
	int num = 1253;
	SCI_val_println(num);
	while(1){
	}
}
//}
//list[PC_view][Tera Term上での表示]{
Hello,world
1253
//}
//listnum[sci_gloval][SCI用グローバル変数][c]{
struct Sci_fifo {
	short index_read;	//読み出し位置
	short index_write;	//書込み位置
	unsigned char buffer[BUFFER_SIZE];	//送信するデータを蓄積する配列
};
static struct Sci_fifo rx_fifo;
static struct Sci_fifo tx_fifo;
//}
//listnum[sci_code][SCI1初期化関数]{
void SCI_init(unsigned long bps) {
	int PCLK = 50;

	// Set MPC
	PORT2.PMR.BIT.B6 = 1;           // P26: peripheral
	PORT3.PMR.BIT.B0 = 1;           // P30: peripheral

	MPC.PWPR.BIT.B0WI = 0;          // Release protect
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.P26PFS.BIT.PSEL = 0x0A;     // Set P26: TXD1
	MPC.P30PFS.BIT.PSEL = 0x0A;     // Set P30: RXD1
	MPC.PWPR.BIT.PFSWE = 0;         // Protect
	MPC.PWPR.BIT.B0WI = 1;

	unsigned char smr;
	unsigned char brr;
	int i;

	// RX FIFO
	rx_fifo.index_read = 0;
	rx_fifo.index_write = 0;

	// TX FIFO
	tx_fifo.index_read = 0;
	tx_fifo.index_write = 0;

	// Bit Rate Register
	brr = (PCLK * 1000 * 1000 / (32 * bps)) - 1;

	SYSTEM.PRCR.WORD = 0xA502;
	MSTP(SCI1) = 0;
	SYSTEM.PRCR.WORD = 0xA500;

	SCI1.SCR.BYTE = 0x00;
	SCI1.SMR.BYTE = 0;			//調歩同期式モードを選択
	SCI1.BRR = brr;

	for (i = 0; i < bps + 100; i++)
				;

	IEN(SCI1, TXI1)= 1;			//TXI割り込み許可
	IEN(SCI1, RXI1)= 1;			//RXI割り込み許可
	IEN(SCI1, TEI1)= 1;			//TEI割り込み許可
	//  IEN(SCI1, ERI1)= 1;

	IPR(SCI1,)= 10; 			//割り込み優先度：10

	SCI1.SCR.BIT.RE = 1;		//シリアル受信動作を許可
	SCI1.SCR.BIT.TE = 1;		//シリアル送信動作を許可
	SCI1.SCR.BIT.RIE = 1;		//RXI割り込み（正常にデータが受信できた際に呼び出される）の許可
}
//}
//listnum[tdr][TDRレジスタ書き込み変数]{
void SCI_put_data_on_TDR(void) {
	unsigned char data = tx_fifo.buffer[tx_fifo.index_read];
	tx_fifo.index_read++;
	if ((tx_fifo.index_read) >= BUFFER_SIZE) {      //配列の末尾にいる場合indexは0へ
		tx_fifo.index_read = 0;
	}
	SCI1.SCR.BIT.TIE = 1;
	SCI1.TDR = data;
}
//}
//listnum[single_c][一文字のみの送信関数]{
void SCI_single_char_transport_TDR(unsigned char input_data) {
	tx_fifo.buffer[tx_fifo.index_write] = input_data; //データを送信バッファに追加
	tx_fifo.index_write++;
	if (tx_fifo.index_write >= BUFFER_SIZE) {        //配列の末尾にいる場合indexは0へ
		tx_fifo.index_write = 0;
	}
	if ((tx_fifo.index_write) != tx_fifo.index_read) { //未送信データがバッファにある場合
		if (SCI1.SCR.BIT.TIE != 1) {
			SCI_put_data_on_TDR();   //データをTDRレジスタに書き込み
		}
	}
}
//}
//listnum[TXI_int][TXI1割り込み（TDRレジスタ送信後発生）の際、起動する変数]{
void TXI1(void) {
	if ((tx_fifo.index_write) != tx_fifo.index_read) { //未送信データがバッファにある場合
		SCI_put_data_on_TDR();   //データをTDRレジスタに書き込み
	} else {
		SCI1.SCR.BIT.TIE = 0;
	}
}
//}

//listnum[transport_string][文字列および整数送信関数]{
void SCI_print(unsigned char *str) {
	while (*str != '\0') {
		SCI_single_char_transport_TDR(*str);
		str++;
	}
}
void SCI_println(unsigned char *str) {
	while (*str != '\0') {
		SCI_single_char_transport_TDR(*str);
		str++;
	}
	SCI_single_char_transport_TDR('\r');
	SCI_single_char_transport_TDR('\n');
}
void SCI_val_print(int i) {
	char num[(int) log10(i) + 1];
	itoa(i, num, 10);
	SCI_print(num);
}
void SCI_val_println(int i) {
	SCI_val_print(i);
	SCI_println("");
}
//}

=== SCI1.SCR
シリアルコントロールレジスタです。このレジスタはクロックの外部出力を行うかどうか、各種通信関係割り込みを許可するかどうかを記述できます。

今回は以下のように設定します。

 * シリアル受信動作：許可
 * シリアル送信動作：許可
 * RXI割り込み：許可

//table[sci_scr][SCI1.SCRレジスタ仕様]{
ビット	シンボル	ビット名	機能	R/W
b1-b0	CKE[1:0]	クロックイネーブルビット	(クロック同期式の場合) @<br>{} b1 b0 @<br>{} 0 x:内部クロック @<br>{} SCKn 端子はクロック出力端子となります @<br>{} 1 x:外部クロック @<br>{} SCKn 端子はクロック入力端子となります	R/W
b2	TEIE	トランスミットエンド @<br>{} インタラプトイネーブルビット	0:TEI 割り込み要求を禁止 @<br>{} 1:TEI 割り込み要求を許可	R/W
b3	MPIE	マルチプロセッサインタラプト@<br>{}イネーブルビット	(調歩同期式モードで、 @<br>{} SMR.MP ビット= “0”とするので無効)	R/W
b4	RE	レシーブイネーブルビット	 @<br>{} 0:シリアル受信動作を禁止 @<br>{} 1:シリアル受信動作を許可	R/W
b5	TE	トランスミット @<br>{} イネーブルビット	 @<br>{} 0:シリアル送信動作を禁止  @<br>{} 1:シリアル送信動作を許可	R/W
b6	RIE	レシーブインタラプトイネーブル @<br>{} ビット	 @<br>{} 0:RXI および ERI 割り込み要求を禁止  @<br>{} 1:RXI および ERI 割り込み要求を許可	R/W
b7	TIE	トランスミットインタラプト @<br>{} イネーブルビット	 @<br>{} 0:TXI 割り込み要求を禁止  @<br>{} 1:TXI 割り込み要求を許可	R/W
//}

=== SCI1.BRR
ビットレートレジスタです。このレジスタはモードやクロックの分周比などを踏まえて値を算出します。

設定情報は

 * モード：調歩同期式
 * SEMR.ABCSビット：0
 * SMR.CKSビット：0
 * モジュールの動作周波数：PCLK = 50(MHz)
 * ボーレート：B=115200(bps)

よって、式は@<eq>{eq_brr}、@<tt>{n=0, B=115200}となります。NはBRRレジスタに代入する値となります。

//texequation[eq_brr][BRR レジスタの設定値 N とビットレート B の関係]{
N = \frac{\rm{PCLK \cdot 10^6}}{64 \cdot 2^{2n-1} \cdot B}
//}


== A/D変換
A/D変換とは端子にかかっている電圧を検出する機能です。
今回の設定仕様を提示します。

 * 使用モジュール：S12ADa
 * A/D変換開始条件：ソフトウェアトリガ
 * AD変換クロック：PCLKB/1
 * 動作モード：シングルスキャンモード

=== AD変換初期化順序
S12ADは逐次比較型のADコンバータです。この仕様のADコンバータは動作原理から分解能のBIT数と同一のクロックカウントを必要とします。ゆえにADコンバータにはクロック入力の設定が必要となります。このときのADコンバータに入力されるクロック１回分を１ステートとユーザーマニュアルは定義しています。
ADコンバータの設定としてはクロックの設定、AD変換のチャンネル選択、モードの選択などを行います。
//image[ad_ini][ADコンバーターの初期化手順][scale=0.4]{
//}

=== AD変換動作順序
今回はCMT0で１Hzの割り込みを発生させ１HzごとにAD変換の値をSCI経由で表示します。
割り込み関数が実行されることに
以下の動作を行います。

//image[ad_cmi][AD変換手順][scale=0.5]{
//}

//listnum[ad_ini_code][AD変換初期化サンプルコード]{
void ad_converter_initialize(void) {

	SYSTEM.PRCR.WORD = 0xA502;      // Release Protect
	MSTP(S12AD) = 0;                // Wake up S12AD
	SYSTEM.PRCR.WORD = 0xA500;      // Protect

	//Set PDR
	PORTE.PDR.BIT.B3 = 0;           // Set PE3: Input

	// Set MPC
	PORTE.PMR.BIT.B3 = 1;           // Set PE3: Peripheral

	MPC.PWPR.BIT.B0WI = 0;          // Release protect
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.PE3PFS.BIT.ASEL = 1;        // Set PE3: Analog Input
	MPC.PWPR.BIT.PFSWE = 0;         // Protect
	MPC.PWPR.BIT.B0WI = 1;

	// S12AD Settings software trigger
	S12AD.ADCSR.BIT.CKS = 3;        //
	S12AD.ADCSR.BIT.ADCS = 0;       // Single scanning
	S12AD.ADANS0.WORD = 0x0000;     // S12ADC: no
	S12AD.ADADS0.WORD = 0x0000;     // Disable ADD result Mode
	S12AD.ADCER.BIT.ACE = 1;        // Auto clearing
	S12AD.ADCER.BIT.ADRFMT = 0;     // Right -justified
	S12AD.ADSSTR01.BIT.SST1 = 20;    // ４00us

}
//}

//listnum[ad_sample_code][AD変換実行時のサンプルコード]{
void ad_print(void) {
           S12AD.ADANS0.BIT.ANS0 = 0x800;
           //AD変換開始
           S12AD.ADCSR.BIT.ADST = 1;
           while ( S12AD.ADCSR.BIT.ADST == 1)
                      ;

           int ad = S12AD.ADDR11;
           int ad_mv = ((float) (ad) * 0.80586081F);
           SCI_print("AD: ");
           SCI_val_println(ad_mv);
}
//}

== SPI
SPI通信はマイコンとデジタルセンサーの間を結ぶデータ通信です。
ジャイロセンサーや加速度センサー、tofセンサーなど数値でデータを受け渡しする必要があるときにSPI通信は役に立つでしょう。
今回の通信対象は磁気式エンコーダーAS5047Dとします。

=== SPI通信とは
クロック同期式のシリアルインターフェースの一種です。マスタとなるマイコン一つに対し複数のICをスレーブとして接続できるインターフェースです。(@<img>{spi_fig})

//image[spi_fig][SPI通信の結線イメージ]{
//}

@<img>{spi_fig}のピンの役割は

 * SCLK 
	  ** 同期するためのクロック信号が流れています。
 * MISO(Master In Slave Out)
	  ** スレーブからマスターにデータを送信する線です。クロックが立ち上がり、立ち下がりどちらかのタイミングで信号線のデータを読みます。
 * MOSI(Master Out Slave In)
 	  ** マスターからスレーブにデータを送信する線です。クロックが立ち上がり、立ち下がりどちらかのタイミングで信号線のデータを読みます。
 * CS（Chip　Select）
  	  ** 通信相手のICを選択するための線です。通信相手（１つのIC）の線はHIGH,他のCSはLOWとします。

となっています。

==== SPI通信のモード
SPI通信には
 * クロックパルスの極性（CPOL）（クロック信号が来るとHIGHになるのかLOWになるのか）
 * クロックパルスのはじめでデータを取るのか後で取るのか（CPHA）
という設定項目があります。

//image[spi_timing][SPI通信の設定項目とタイミングの関係]{
//}

設定項目の組み合わせによってモードの名前が決められています。各種使用するセンサーICのデータシートをよく確認しましょう。
//table[spi_mode][設定項目とモードの関係]{
動作モード	CPOL	CPHA
------------------------------------
モード0	0	0
モード1	0	1
モード2	1	0
モード3	1	1
//}

今回使用するAS5047DはSPI timing diagramの図（@<img>{as_timing}）を見るとモード１です。
//image[as_timing][AS5047Dの通信タイミング]{
//}
ではSPI通信の設定に移りましょう。

=== SPI通信の設定
今回設定する仕様を提示します。

 * 使用するモジュール：RSPI0
 * SPI通信のモード：モード１
 * データ長：16bit
 * データ転送の順番：MSBファースト（最上位ビットから順番に転送）
 * 磁気式エンコーダーAD5047Dからの取得データ：14bit

=== 初期化設定の流れ
//image[spi_ini_scheme][RSPIモジュールの初期化の流れ][scale=0.5]{
//}

=== データ通信の流れ
受信待機を行わなければいけない都合上、通信プログラムはmainと割り込み関数に存在します。
@<img>{spi_main}をよく見ると、同一の操作を繰り返す箇所があります。これはAD5047Dの仕様で１度目のデータ送信で送ったアドレスの内容を２回目に返信します。ですのでこのセンサーからデータ読み出すには２度のデータ送信を行う必要があります。

//image[spi_main][データ受信プログラム本体][scale=0.5]{
//}

//image[spi_interruption][受信割り込み関数内の受信確認プログラムの流れ][scale=0.5]{
//}


//listnum[spi_code][SPI初期化関数]{
void SPI_init(void) {

	PORTC.PDR.BIT.B5 = 1;           //CLK
	PORTC.PDR.BIT.B6 = 1;           //MOSI
	PORTC.PDR.BIT.B7 = 0;           //MISO

	PORTC.PMR.BIT.B5 = 1;           //CLK
	PORTC.PMR.BIT.B6 = 1;           //MOSI
	PORTC.PMR.BIT.B7 = 1;           //MISO

	//mpcの書き込み許可
	MPC.PWPR.BIT.B0WI = 0;          //PFSWEビット書き込み許可
	MPC.PWPR.BIT.PFSWE = 1;         //PFSレジスタへ書き込み許可

	//SPI端子設定
	MPC.PC5PFS.BIT.PSEL = 0x0D;     //RSPCLK
	MPC.PC6PFS.BIT.PSEL = 0x0D;     //MOSI
	MPC.PC7PFS.BIT.PSEL = 0x0D;     //MISO

	MPC.PWPR.BIT.PFSWE = 0;         //PFSレジスタへ書き込み禁止
	MPC.PWPR.BIT.B0WI = 1;          //PFSWEビット書き込み禁止

	PORTA.PDR.BIT.B4 = 1;   //CS: ENCODER

// Wake up RSPI0
	SYSTEM.PRCR.WORD = 0xA502;
	MSTP(RSPI0) = 0;
	SYSTEM.PRCR.WORD = 0xA500;

// SPI機能の停止
	RSPI0.SPCR.BIT.SPE = 0;

// MOSI idle set high
	RSPI0.SPPCR.BIT.MOIFE = 1;
	RSPI0.SPPCR.BIT.MOIFV = 1;

// set for bitrate
	RSPI0.SPBR = 1;

	RSPI0.SPDCR.BIT.SPLW = 1;

	RSPI0.SPSCR.BIT.SPSLN = 2;
	RSPI0.SPDCR.BIT.SPFC = 0;

	RSPI0.SPCMD0.WORD = 0x0F0D;
	RSPI0.SPCMD1.WORD = 0x0F0D;

	IPR(RSPI0,)= 2;

	IEN(RSPI0, SPRI0)= 1;
	IEN(RSPI0, SPTI0)= 1;

	RSPI0.SPCR.BIT.SPEIE = 1;
	RSPI0.SPCR.BIT.SPTIE = 0;
	RSPI0.SPCR.BIT.SPRIE = 1;
	RSPI0.SPCR.BIT.MSTR = 1; 
}

//}

//listnum[spi_getdata][SPI通信におけるICからのデータ伝送プログラム]{
uint16_t SPI_Encoder_get(void) {

           RSPI0.SPCR.BIT.SPE = 1;
           while (RSPI0.SPCR.BIT.SPE == 0)
                      ;

           PORTA.PODR.BIT.B4 = 0;   //CS: ENCODER
           volatile int i = 0;
           for (i = 0; i < 50; i++) {
           }
           put_SPDR(0x3FFF);

           for (i = 0; i < 50; i++) {
           }
           PORTA.PODR.BIT.B4 = 1;   //CS: ENCODER

           for (i = 0; i < 100; i++) {
           }

           PORTA.PODR.BIT.B4 = 0;   //CS: ENCODER
           for (i = 0; i < 50; i++) {
           }
           uint16_t receive = put_SPDR(0x0000);
           PORTA.PODR.BIT.B4 = 1;   //CS: ENCODER

           for (i = 0; i < 100; i++) {
           }
           PORTA.PODR.BIT.B4 = 1;   //CS: ENCODER

           RSPI0.SPCR.BIT.SPE = 0;
           while (RSPI0.SPCR.BIT.SPE == 1)
                      ;

           return receive & 0x3FFF;
}
//}

=== レジスタの説明
==== RSPI0.SPCR
RSPI制御レジスタです。このレジスタは割り込みの許可や通信モードの設定を行うレジスタです。
RSPIの設定を変更する際にはこのレジスタでRSPI機能を停止してから行います。

==== RSPI0.SPPCR
RSPI端子制御レジスタです。このレジスタではMOSI端子の待機状態での出力設定、ループバックモードなどの設定を行います。

==== RSPI0.SPBR
RSPIビットレートレジスタです。CLK端子から出力されるクロック信号の周期を決定します。通信相手のICのクロックの上限値を確認した上で設定しましょう。

==== RSPI0.SPDCR
RSPIデータコントロールレジスタです。設定項目は以下の３つです。
 * SPFC（フレーム数設定ビット）
 ** SPDRレジスタに格納できるフレーム数を設定します。今回のセンサーは１シーケンスで送信するフレーム数は１つです。
 * SPRDTD（RSPI 受信 / 送信データ選択 ビット）
 ** SPDRレジスタの読み出すデータを送信バッファとするか、受信バッファとするかを設定します。
 * SPLW（RSPI ロングワードアクセス / ワードアクセス設定ビット）
 ** SPDRレジスタのアクセス幅（データの大きさ）を設定するレジスタです。