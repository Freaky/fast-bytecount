use std::time::{Duration, Instant};
use std::fs::read;
use bytecount;

fn main() {
	let buf = read("../sherlock-holmes-huge.txt").expect("file");

	let mut count = 0;
	let mut cum_dur = Duration::from_secs(0);

	for i in 0..255 {

		print!("{}\t", i);

		let start = Instant::now();
		for _ in 0..512 {
			count += bytecount::count(&buf, i as u8);
		}
		let el = start.elapsed();
		cum_dur += el;

		println!("{:.2?}", el);
	}



	println!("Counted {} in {:.2}", count, cum_dur.as_secs() as f64 * 1000.0 + (cum_dur.subsec_nanos() as f64 / 1_000_000.0));
}
