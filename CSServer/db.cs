using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MySql.Data.MySqlClient;

namespace CSServer
{
    public sealed class Db
    {
        private readonly string _connStr;

        public Db(string connectionString)
        {
            _connStr = connectionString;
        }

        public static readonly Db Global = CreateDefault();

        public static Db CreateDefault()
        {
            var dbInfo = new MySqlConnectionStringBuilder
            {
                Server = "127.0.0.1",
                Port = 3306,
                Database = "can_qc",
                UserID = "root",
                Password = "1234",
                SslMode = MySqlSslMode.Disabled,
            };
            return new Db(dbInfo.ConnectionString);
        }

        // 1) 테이블 보장
        public async Task EnsureTableAsync()
        {
            // DB 테이블 변경시 하단 코드 손봐야함. ALTER , 마이그레이션으로 처리하는 걸 추천했었음. 
            const string sql = @"
CREATE TABLE IF NOT EXISTS DETECT_RESULT (
  id          BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  imgid       INT            NOT NULL,
  created_at  DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,
  result      VARCHAR(24)    NULL,
  image_path  VARCHAR(512)   NOT NULL,
  INDEX idx_imgid_created (imgid, created_at)
) ENGINE=InnoDB;";

            await using var conn = new MySqlConnection(_connStr);
            await conn.OpenAsync();

            await using var cmd = new MySqlCommand(sql, conn);
            await cmd.ExecuteNonQueryAsync();
        }

        // 2) 이미지 경로 저장 (중복검사 후 그래도 추가)
        public async Task InsertImageAsync(int imgid, string imagePath)
        {
            await using var conn = new MySqlConnection(_connStr);
            await conn.OpenAsync();

            int dupCount;
            await using (var check = new MySqlCommand(
                "SELECT COUNT(*) FROM DETECT_RESULT WHERE imgid=@imgid;", conn))
            {
                check.Parameters.AddWithValue("@imgid", imgid);
                dupCount = Convert.ToInt32(await check.ExecuteScalarAsync());
            }

            if (dupCount > 0)
                Console.WriteLine($"[DB] 중복된 imgid가 있습니다. (imgid={imgid}, 기존개수={dupCount})");

            await using (var insert = new MySqlCommand(
                "INSERT INTO DETECT_RESULT (imgid, image_path) VALUES (@imgid, @path);", conn))
            {
                insert.Parameters.AddWithValue("@imgid", imgid);
                insert.Parameters.AddWithValue("@path", imagePath);
                await insert.ExecuteNonQueryAsync();
            }
        }

        // 3) 결과 업데이트: 같은 imgid 중 최근 1행만 PASS/FAIL로 변경
        public async Task<int> UpdateResultAsync(int imgid, bool isPass)
        {
            string resultText = isPass ? "PASS" : "FAIL";
            const string sql = @"
UPDATE DETECT_RESULT
SET result = @result
WHERE imgid = @imgid
ORDER BY id DESC
LIMIT 1;";

            await using var conn = new MySqlConnection(_connStr);
            await conn.OpenAsync();
            await using var cmd = new MySqlCommand(sql, conn);
            cmd.Parameters.AddWithValue("@result", resultText);
            cmd.Parameters.AddWithValue("@imgid", imgid);

            int affected = await cmd.ExecuteNonQueryAsync();
            if (affected == 0)
                Console.WriteLine($"[DB] imgid={imgid} 갱신 대상 행이 없습니다.");

            return affected;
        }
    }
}
