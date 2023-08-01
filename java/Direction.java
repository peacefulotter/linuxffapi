
enum Direction {
   LEFT(0x4000),
   RIGHT(0xC000);

   public final int value;

   Direction(int value) {
      this.value = value;
   }
}