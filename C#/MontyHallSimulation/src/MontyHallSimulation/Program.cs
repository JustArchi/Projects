/*
       _           _      _             _     _
      | |_   _ ___| |_   / \   _ __ ___| |__ (_)
   _  | | | | / __| __| / _ \ | '__/ __| '_ \| |
  | |_| | |_| \__ \ |_ / ___ \| | | (__| | | | |
   \___/ \__,_|___/\__/_/   \_\_|  \___|_| |_|_|

  Copyright 2017 Łukasz "JustArchi" Domeradzki
  Contact: JustArchi@JustArchi.net

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

*/

using System;
using System.Diagnostics.CodeAnalysis;

namespace MontyHallSimulation {
	internal static class Program {
		private static readonly Random Random = new Random();

		private static void Test(ushort choices, uint rounds) {
			if (choices < 2) {
				throw new ArgumentException(nameof(choices));
			}

			if (rounds == 0) {
				throw new ArgumentException(nameof(rounds));
			}

			uint winsKeepingChoice = 0;
			uint winsChangingChoice = 0;

			for (uint round = 0; round < rounds; round++) {
				ushort winningDoor = (ushort) Random.Next(choices);
				ushort ourDoor = (ushort) Random.Next(choices);

				// Host opens all doors but 2 now, never opening the door with the prize and never opening the door that we picked, so possible variants:
				// a) winningDoor is the door we picked, second one is empty
				// b) winningDoor is not the door we picked, second one holds the prize
				if (winningDoor == ourDoor) {
					winsKeepingChoice++;
				} else {
					winsChangingChoice++;
				}
			}

			Console.WriteLine("With " + choices + " possible choices, after " + rounds + " rounds: ");
			Console.WriteLine("We'd win " + winsKeepingChoice + "/" + rounds + " rounds (" + (winsKeepingChoice / (float) rounds).ToString("P2") + ") if we kept our initial choice");
			Console.WriteLine("We'd win " + winsChangingChoice + "/" + rounds + " rounds (" + (winsChangingChoice / (float) rounds).ToString("P2") + ") if we changed our initial choice");
		}

		[SuppressMessage("ReSharper", "FunctionNeverReturns")]
		private static void Main() {
			while (true) {
				Console.Write("Enter number of choices (doors), range 2-" + ushort.MaxValue + ": ");
				string choicesString = Console.ReadLine();

				ushort choices;
				if (!ushort.TryParse(choicesString, out choices) || (choices < 2)) {
					Console.WriteLine("Invalid number, try again!");
					Console.WriteLine("-----");
					continue;
				}

				Console.Write("Enter number of rounds (attempts), range 1-" + uint.MaxValue + ": ");
				string roundsString = Console.ReadLine();

				uint rounds;
				if (!uint.TryParse(roundsString, out rounds) || (rounds == 0)) {
					Console.WriteLine("Invalid number, try again!");
					Console.WriteLine("-----");
					continue;
				}

				Console.WriteLine("-----");
				Test(choices, rounds);
				Console.WriteLine("-----");
			}
		}
	}
}
